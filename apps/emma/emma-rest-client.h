#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "rest.h"
#include "buffer.h"

#define EMMA_REST_CLIENT_REQUEST_MAX 1
#define EMMA_REST_CLIENT_REQUEST_TIMEOUT 1 * CLOCK_SECOND
#define EMMA_REST_CLIENT_REQUEST_TIMEOUT_CHECK 1 * CLOCK_SECOND

#define EMMA_SEND(urls, callback, method, body)\
emma_send(urls, callback, method, body)
//emma_prepare(url, callback, method, body)

enum{
//NO_ERROR, 					// Define in coap-common.h
//MEMORY_ALLOC_ERR,		// Define in coap-common.h
PROCESS_ERR=2,
TIMEOUT
};

enum {
  GET = 1,
  POST,
  PUT,
  DELETE
};

typedef struct emma_request_t emma_request_t;
typedef void (*client_handler) (error_t error, struct emma_request_t*, char* rsp);

struct emma_request_t{
	/* engine parameters*/
	struct emma_request_t* next;
	struct uip_udp_conn * client_conn;
	client_handler handler;
	clock_time_t timestamp;	
	
	/* target */
	uip_ipaddr_t server_ipaddr;
	u16_t port;
	char uri[EMMA_URL_SIZE_MAX];
	method_t method;
	char body[EMMA_PAYLOAD_SIZE_MAX];
	
};

void
emma_send(char * url, void * callback, method_t method, char * body);

emma_error_t
emma_make_target(char* req, emma_request_t* t);

void emma_prepare(char * url, void * callback, method_t method, char * body);

PROCESS_NAME(emma_res_client);
