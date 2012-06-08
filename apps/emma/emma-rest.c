#include "emma-rest.h"
#include "emma-rest-client.h"
#include "emma-resources.h"
#include "emma-agent.h"
#include "emma-vm.h"
#include "emma-util.h"

#include "net/uip-nd6.h"
#include "net/uip-ds6.h"


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

extern uip_ds6_nbr_t uip_ds6_nbr_cache[UIP_DS6_NBR_NB]; 

/*************************************************
AGENTS RESOURCE
**************************************************/
RESOURCE(agents, METHOD_GET | METHOD_POST | METHOD_DELETE, ".well-known/agents");

void 
agents_handler(REQUEST* request, RESPONSE* response)
{
	int res = 0;
	char name[AGENT_NAME_SIZE];
	char pre[AGENT_PRE_SIZE];
	char target[AGENT_TARGET_SIZE];
	char post[AGENT_POST_SIZE];
	
	// Switch action
	switch(rest_get_method_type(request))
		{			
		case METHOD_GET :
	
				
			rest_set_response_payload(response, (uint8_t*)(resource_agents.user_data), strlen(((char*)resource_agents.user_data)));
  		rest_set_header_content_type(response, APPLICATION_LINK_FORMAT);
			break;		
			
		case METHOD_POST :
			if(!rest_get_post_variable(request, "name", 	name, 	sizeof(name))) 		res = -1;
			if(!rest_get_post_variable(request, "pre" , 	pre,  	sizeof(pre)))  		res = -1;
			if(!rest_get_post_variable(request, "target", target, sizeof(target)))  res = -1;
			if(!rest_get_post_variable(request, "post", 	post, 	sizeof(post))) 		res = -1;
			
			if(res == 0) res = agent_new(name, pre, target, post);
			break;
			
		case METHOD_DELETE :
			if(!rest_get_post_variable(request, "name", name, sizeof(name))) res = -1;	
			
			if(res == 0) res = agent_delete(name);
			break;
			
		default : ;
		}  
		
	// Send response
	if(res == 0)	rest_set_response_status(response, 200);
	else					rest_set_response_status(response, 500);
}

int 
agents_build(){
  int index = 0;
	agent_t* agent = NULL;
  
	for (		agent = (agent_t*)list_head(((list_t)agent_get_all())); 
					agent && strlen(resource_agents.user_data) < BUFF_SIZE; 
					agent = agent->next)
		index += sprintf(resource_agents.user_data + index, "</agent>;n=\"%s\",", agent->name);	
  	
	((char*)resource_agents.user_data)[index -1] = '\0';
	return 1;
	}
/*************************************************
DISCOVER RESOURCE
**************************************************/
RESOURCE(discover, METHOD_GET, ".well-known/core");

void
discover_handler(REQUEST* request, RESPONSE* response)
{
  rest_set_response_payload(response, (uint8_t*)resource_discover.user_data, strlen(resource_discover.user_data));
  rest_set_header_content_type(response, APPLICATION_LINK_FORMAT);
}

int 
discover_build(){
  int index = 0;
  
  resource_t* resource = NULL;
  for (		resource = (resource_t*)list_head(rest_get_resources()); 
  				resource && strlen(resource_discover.user_data) < BUFF_SIZE; 
  				resource = resource->next)
  	index += sprintf(resource_discover.user_data + index, "</resource>;n=\"%s\",", resource->url);
  	
	((char*)resource_discover.user_data)[index -1] = '\0';
	return 1;
	}

/*************************************************
NEIGHBORHOOD RESOURCE
**************************************************/
RESOURCE(neighborhood, METHOD_GET, "neighborhood");
void
neighborhood_handler(REQUEST* request, RESPONSE* response)
{
  rest_set_response_payload(response, (uint8_t*)resource_neighborhood.user_data, strlen(resource_neighborhood.user_data));
  rest_set_header_content_type(response, APPLICATION_LINK_FORMAT);
}

int
neighborhood_build(REQUEST* request, RESPONSE* response){
	static uip_ds6_nbr_t *locnbr;
  int index = 0;
  for(locnbr = uip_ds6_nbr_cache;
      locnbr < uip_ds6_nbr_cache + UIP_DS6_NBR_NB;
      locnbr++)
      if(locnbr->isused){
  			index += sprintf(resource_neighborhood.user_data + index, "</host>;n=\"%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\",", 
  				((u8_t *)&locnbr->ipaddr)[0], 
  				((u8_t *)&locnbr->ipaddr)[1], 
  				((u8_t *)&locnbr->ipaddr)[2], 
  				((u8_t *)&locnbr->ipaddr)[3], 
  				((u8_t *)&locnbr->ipaddr)[4], 
  				((u8_t *)&locnbr->ipaddr)[5], 
  				((u8_t *)&locnbr->ipaddr)[6], 
  				((u8_t *)&locnbr->ipaddr)[7], 
  				((u8_t *)&locnbr->ipaddr)[8], 
  				((u8_t *)&locnbr->ipaddr)[9], 
  				((u8_t *)&locnbr->ipaddr)[10], 
  				((u8_t *)&locnbr->ipaddr)[11], 
  				((u8_t *)&locnbr->ipaddr)[12], 
  				((u8_t *)&locnbr->ipaddr)[13], 
  				((u8_t *)&locnbr->ipaddr)[14], 
  				((u8_t *)&locnbr->ipaddr)[15]);
				}
	((char*)resource_neighborhood.user_data)[index -1] = '\0';
	return 1;
	}

/*************************************************
STANDARD RESSOURCE
**************************************************/
void 
handler(REQUEST* request, RESPONSE* response)
{
	char buff[BUFF_SIZE];
	buff[0] = '\0';
	char * tmp = NULL;
	int res = -1;

	// GET Ressource
	strncpy(buff,request->url, request->url_len);
	buff[request->url_len] = '\0';
	
	resource_t * r = rest_get_resource(buff);
	if(r == NULL){
		PRINTF("REST : RESSOURCE %s not found\n",buff);
		rest_set_response_status(response, 404);
		return;
		}
		
	// Switch action
	switch(rest_get_method_type(request))
		{
		case METHOD_GET : 
				tmp = rest_get_user_data(r);
			break;
		
		case METHOD_PUT :
			if(rest_get_post_variable(request, "value", buff, sizeof(buff))){
				res = vm_eval(buff);
				printf("ICI : %s | %d\n", buff, res);
				sprintf(r->user_data, "%d", res);
				}
			else rest_set_response_status(response, 400);
			break;
			
		default : ;
		}  
		
	// Send response
	if(tmp != NULL)  rest_set_response_payload(response, (uint8_t*)tmp, strlen(tmp));
  rest_set_header_content_type(response, TEXT_PLAIN);
}

/* Pre handler for resource request */
int
pre(REQUEST* request, RESPONSE* response){
	// Save in log
	if (request != NULL){
		char url[BUFF_SIZE];
		strncpy(url, request->url, request->url_len);
		url[request->url_len] = '\0';
		PRINTF("REST : Pre-handler of %s\n", url);
		}
	return 1;
	}
	
/* Post handler for resource request */
void 
post(REQUEST* request, RESPONSE* response){
	char url[BUFF_SIZE];
	strncpy(url, request->url, request->url_len);
	url[request->url_len] = '\0';
	
	resource_t* r = rest_get_resource(url);
	if(r==NULL) return;
	agent_run_all();
	}
	
/**************************************************
E.M.M.A Init
**************************************************/
void 
emma_init(){
	#ifdef WITH_COAP
		PRINTF("COAP Server\n");
	#else
		PRINTF("HTTP Server\n");
	#endif
  rest_init();
  resources_init();
  
  rest_activate_resource(&resource_agents);
  resource_agents.pre_handler = &agents_build;
  resource_agents.user_data		= malloc(BUFF_SIZE*sizeof(char));
  
  rest_activate_resource(&resource_discover);
  resource_discover.pre_handler = &discover_build;
  resource_discover.user_data		= malloc(BUFF_SIZE*sizeof(char));
  
  rest_activate_resource(&resource_neighborhood);
  resource_neighborhood.pre_handler = &neighborhood_build;
  resource_neighborhood.user_data		= malloc(BUFF_SIZE*sizeof(char));
  
  process_start(&emma_res_client, NULL);
}
