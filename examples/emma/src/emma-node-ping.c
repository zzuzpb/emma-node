#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "emma-rest.h"

#include "emma-rest-client.h"
#include "emma-util.h"

EMMA_RESSOURCE(brightness);

PROCESS(emma_node, "Emma node");
AUTOSTART_PROCESSES(&emma_node);


int value = 0;
void 
response(error_t err, emma_request_t* req, char* rsp){
	if(err == NO_ERROR){
		printf("APPS : [SUCCESS] Payload : %s\n",rsp);
		++value;
		}
	else printf("APPS : [ERROR] error : %d\n", err);
	}

PROCESS_THREAD(emma_node, ev, data)
{
	static struct etimer et;
	
  PROCESS_BEGIN();
	EMMA_INIT();
	EMMA_ACTIVATE(brightness, "0");
	char body[10];

	printf("PING-PONG\n");

	#if !RPL_BORDER_ROUTER
	etimer_set(&et, 1 *CLOCK_SECOND);
	while(1) {
    PROCESS_YIELD();
    
    if (etimer_expired(&et)) {
    	sprintf(body, "value=%d", value);
			#ifdef CLIENT1
				#define TARGET 
				EMMA_SEND("bbbb:0000:0000:0000:0000:00ff:fe00:0020/brightness", response, (value%2 ? PUT : GET), (value%2?body:NULL));
			#else 
				EMMA_SEND("bbbb:0000:0000:0000:0000:00ff:fe00:0010/brightness", response, (value%2 ? PUT : GET), (value%2?body:NULL));
			#endif
    	
      etimer_reset(&et);
    	} 
    	
  	}
  #endif
	
  PROCESS_END();
}
