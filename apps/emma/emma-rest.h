#ifndef H_EMMA_REST
#define H_EMMA_REST

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "contiki.h"
#include "contiki-net.h"
#include "rest.h"

#define DEFAULT_PORT 						61616
#define LOCAL_PORT 							61617

#define BUFF_SIZE 							COAP_DATA_BUFF_SIZE
#define EMMA_URL_SIZE_MAX 			COAP_DATA_BUFF_SIZE
#define EMMA_PAYLOAD_SIZE_MAX 	COAP_DATA_BUFF_SIZE
#define MAX_PAYLOAD_LEN   			COAP_DATA_BUFF_SIZE

#define EMMA_RESSOURCE(name)\
char resource_ ## name ## _data[BUFF_SIZE];\
resource_t resource_ ## name = {NULL, METHOD_GET | METHOD_PUT, #name, handler, pre, post, (void*)&resource_ ## name ## _data}

#define EMMA_INIT()\
emma_init()

#define EMMA_ACTIVATE(name,value)\
sprintf(resource_ ## name .user_data,"%s",value);\
rest_activate_resource(&resource_ ## name);


enum emma_error_t{
EMPTY_ARGUMENT=1,
NOT_ENOUGH_MEMORY,
RESOURCE_NOT_FOUND,
RESOURCE_ALREADY_EXIST,
AGENT_NOT_FOUND,
URI_ERROR
};
typedef enum emma_error_t emma_error_t;


void emma_init();
void handler(REQUEST* request, RESPONSE* response);
int pre(REQUEST* request, RESPONSE* response);
void post(REQUEST* request, RESPONSE* response);

#endif 
