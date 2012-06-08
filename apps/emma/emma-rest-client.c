#include "emma-rest.h"
#include "emma-rest-client.h"

#include "contiki.h"
#include "contiki-net.h"
#include "emma-util.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((u8_t *)addr)[0], ((u8_t *)addr)[1], ((u8_t *)addr)[2], ((u8_t *)addr)[3], ((u8_t *)addr)[4], ((u8_t *)addr)[5], ((u8_t *)addr)[6], ((u8_t *)addr)[7], ((u8_t *)addr)[8], ((u8_t *)addr)[9], ((u8_t *)addr)[10], ((u8_t *)addr)[11], ((u8_t *)addr)[12], ((u8_t *)addr)[13], ((u8_t *)addr)[14], ((u8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF(" %02x:%02x:%02x:%02x:%02x:%02x ",(lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3],(lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

#ifdef WITH_COAP
#define RESOURCE_LABEL_SIZE 15
const char* resource_label = {"</resource>;n=\"\0"};

int xact_id;
struct etimer et_timeout;
LIST(requests_table);
MEMB(requests, emma_request_t, EMMA_REST_CLIENT_REQUEST_MAX);
process_event_t emma_rest_client_event_send;

/********************************************************
void 
emma_send(char * urls, void * callback, method_t method, char * body)
********************************************************/
void
emma_send(char * urls, void * callback, method_t method, char * body){

	char* tmp = NULL;
	char* tmp2 = NULL;
	char buf[BUFF_SIZE];
	
	tmp = strstr(urls, resource_label);
	if (tmp == NULL)
		emma_prepare(urls, callback, method, body);
	
	else {
		do{
			// Extraction of target among the list of resources
			tmp +=  RESOURCE_LABEL_SIZE;
			tmp2 = strstr(tmp,",");
			if(tmp2 != NULL) {
				strncpy(buf, tmp, tmp2-tmp);
				buf[tmp2-tmp] = '\0';
				}
			else strcpy(buf, tmp);		
			buf[strlen(buf)-1] = '\0';
			
			PRINTF("Multi-sending %s\n", buf);
			emma_prepare(buf, callback, method, body);
			
			if(tmp != NULL)
				tmp = strstr(tmp, resource_label);
			}
		while(tmp != NULL);
		}
	}
/********************************************************
void 
emma_prepare(char * url, void * callback, method_t method, char * body)
* Prepare the request and send it to emma_res_client process 
* by the process event 

void 
send(emma_request_t * req)
* Send the request to uip stack
********************************************************/
void 
emma_prepare(char * url, void * callback, method_t method, char * body){	
	emma_request_t * req = (emma_request_t*) memb_alloc(&requests);
	if(req == NULL){
		((client_handler)callback) (MEMORY_ALLOC_ERR, req, NULL);
		PRINTF("REST-CLIENT: Not enough MEMB space\n");
		return;
		}
	list_add(requests_table, req);
		
	if(emma_make_target(url, req) == URI_ERROR){
		((client_handler)callback) (URI_ERROR, req, NULL);
		PRINTF("REST-CLIENT: REST-CLIENT: URI bad format\n");
		return;
		}
	
	req->method 	= method ? method : req->method;
	req->handler 	= callback;
	if(body != NULL)
		sprintf(req->body,"%s", body);
		
	int res = process_post 	(&emma_res_client, emma_rest_client_event_send, req);
	if (res != PROCESS_ERR_OK && req->handler) 
		req->handler(PROCESS_ERR, req, NULL);
	};
/********************************************************/
void 
send(emma_request_t * req){

 	req->client_conn = udp_new(&req->server_ipaddr, UIP_HTONS(req->port), req);
 	
 	if(req->client_conn == NULL){
		if(req->handler)	req->handler(MEMORY_ALLOC_ERR, req, NULL);
		PRINTF("REST-CLIENT: Not enough UDP memory space\n");
		return;
		}

  udp_bind(req->client_conn, UIP_HTONS(LOCAL_PORT));

  char buf[MAX_PAYLOAD_LEN];

  if (init_buffer(COAP_DATA_BUFF_SIZE)) {
    int data_size = 0;
    coap_packet_t* request = (coap_packet_t*)allocate_buffer(sizeof(coap_packet_t));
    init_packet(request);
    
    coap_set_method(request, req->method ? req->method : GET);
    request->tid = xact_id++;
    request->type = MESSAGE_TYPE_CON;
    coap_set_header_uri(request, req->uri);
		coap_set_payload(request, (u8_t*)req->body, strlen(req->body));
		
    data_size = serialize_packet(request, (u8_t*)buf);

    PRINTF("REST-CLIENT: Client sending request to:[");
    PRINT6ADDR(&req->client_conn->ripaddr);
    PRINTF("]:%u/%s\n", req->port, req->uri);
    uip_udp_packet_send(req->client_conn, buf, data_size);
		req->timestamp = clock_time ();

    delete_buffer();
  	}
  else {
  	if(req->handler)	req->handler(MEMORY_ALLOC_ERR, req, NULL);
  	PRINTF("REST-CLIENT: NOT ENOUGH MEMORY\n");
  	if(req->client_conn)
	  	uip_udp_remove(req->client_conn);
		list_remove(requests_table, req);
		memb_free (&requests, req);
		}

	};

/********************************************************
* 
********************************************************/
void
response_handler(coap_packet_t* response, emma_request_t* req)
{
  uint16_t payload_len = 0;
  uint8_t* payload = NULL;
  payload_len = coap_get_payload(response, &payload);
	
  PRINTF("REST-CLIENT: Response transaction id: %u\n", response->tid);
  if (payload) {
  	payload[payload_len] = '\0';
    PRINTF("REST-CLIENT:  payload: %s\n", payload);
  	}
  if(req->handler) 
  	req->handler(NO_ERROR, req, (char*)payload);
  
  if(req->client_conn)
	 	uip_udp_remove(req->client_conn);
	
	list_remove(requests_table, req);
	memb_free (&requests, req);
}

void
emma_handle_incoming_data(emma_request_t* data)
{
  PRINTF("REST-CLIENT: Incoming packet size: %u \n", (u16_t)uip_datalen());
  if (init_buffer(COAP_DATA_BUFF_SIZE)) {
    if (uip_newdata()) {
      coap_packet_t* response = (coap_packet_t*)allocate_buffer(sizeof(coap_packet_t));
      if (response) {
        parse_message(response, uip_appdata, uip_datalen());
        response_handler(response, data);
      }
    }
    delete_buffer();
  }
}

/********************************************************
* 
********************************************************/
void 
emma_handle_timer_timeout(){
  clock_time_t now;
  now = clock_time();
	emma_request_t* req = NULL;
  for (		req = (emma_request_t*)list_head(requests_table); 
  				req; 	req = req->next){
  		if(req->timestamp + EMMA_REST_CLIENT_REQUEST_TIMEOUT < now){
  			PRINTF("REST-CLIENT: TIMEOUT %s\n", req->uri);
					
				if(req->client_conn)
					uip_udp_remove(req->client_conn);
					
				if(req->handler) 
					req->handler(TIMEOUT, req, NULL);						 	
					
				list_remove(requests_table, req);
				memb_free (&requests, req);
				req = (emma_request_t*)list_head(requests_table);
				if(!req)
					return;
  			}		
  	}
	}	
	
PROCESS(emma_res_client, "Emma client");
PROCESS_THREAD(emma_res_client, ev, data)
{
  PROCESS_BEGIN();
  PRINTF("REST-CLIENT: Init\n");
  
	emma_rest_client_event_send = process_alloc_event();
	memb_init(&requests);
	list_init(requests_table);
	
  etimer_set(&et_timeout, EMMA_REST_CLIENT_REQUEST_TIMEOUT_CHECK);
	while(1) {
		PROCESS_YIELD();
		
		if (ev == tcpip_event) 
      emma_handle_incoming_data(((emma_request_t*) data));
    	
    else if (ev ==  emma_rest_client_event_send)
    	send((emma_request_t*)data);
    	
    else if (etimer_expired(&et_timeout)){
    	emma_handle_timer_timeout();
    	etimer_reset(&et_timeout);
    	}
    } 
	PROCESS_END();
}	

/********************************************************/
/* BUILD Request */
/********************************************************/
emma_error_t 
emma_make_target(char* req, emma_request_t* t){
	char * tmp = NULL;
	char temp[42];
	
	tmp = strstr (req,"/");	
	if(tmp == NULL){
		req[strlen(req)] = '/';
		tmp = strstr (req,"/");	
		}
		
	if(tmp-req >=40)
		return URI_ERROR;
		
	strncpy(temp, req, tmp-req);
	temp[tmp-req] = '\0';
	uiplib_ipaddrconv(temp, &t->server_ipaddr);
	PRINT6ADDR(&t->server_ipaddr);
	
	*tmp++;
	sprintf(t->uri,"%s", tmp);
	t->port = DEFAULT_PORT;
	t->method = METHOD_GET;
	t->body[0] = '\0';
	
	return 0;
}


#endif
