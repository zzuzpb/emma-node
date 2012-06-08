#ifdef EMMA_SHELL 

#include "contiki.h"
#include "dev/serial-line.h"
#include "shell.h"
#include "serial-shell.h"
#include "emma-rest.h"
#include "emma-util.h"
#include "emma-agent.h"
#include "rest.h"
#include "emma-resources.h"
#include "emma-vm.h"
#include "emma-rest-client.h"

extern struct memb resources_R;

#ifndef __AVR__
	#define _P(s) s 
	#define PSTR(s) s
#else 
	#include <avr/pgmspace.h>
#endif

#define DEBUG 0
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

void
emma_shell_print_res(emma_error_t fail, struct shell_command* proc, void (usage)()){
	switch(fail){
		case EMPTY_ARGUMENT : 
			shell_output_str(proc, "ERROR: ", "EMPTY ARGUMENT");
			if(usage != NULL) usage();
			break;
			
		case RESOURCE_NOT_FOUND : 
			shell_output_str(proc, "ERROR: ", "RESOURCE NOT FOUND");
			if(usage != NULL) usage();
			break;
			
		case RESOURCE_ALREADY_EXIST :
			shell_output_str(proc, "ERROR: ", "RESOURCE ALREADY EXIST");
			if(usage != NULL) usage();
			break;
			
		case AGENT_NOT_FOUND :
			shell_output_str(proc, "ERROR: ", "AGENT NOT FOUND");
			if(usage != NULL) usage();
			break;
		default : ;
};
	}

/***********************************************************/
/* GET */
/***********************************************************/
PROCESS(emma_shell_get_process, "eget");
SHELL_COMMAND(emma_shell_get_command,
	      "eget",
	      "eget: get ressource value (eget uri)",
	      &emma_shell_get_process);
/***********************************************************/
void 
emma_shell_get_usage(){
  shell_output_str(&emma_shell_get_command, "USAGE: ", "eget uri");
}
/***********************************************************/
void
emma_shell_get(char* cmd){
  resource_t * r 	= NULL;			
  agent_t *	ag		= NULL; 
  char * tmp 			= NULL;
  char * tmp2			= NULL;
  int fail=0;
  
  tmp	=	strtok(cmd," ");
  if(tmp == NULL)	fail = EMPTY_ARGUMENT;
    
  if(!fail){
  	// Check if agent request
  	if(!strncmp(tmp,"agents",6) || !strncmp(tmp,"/agents",7)){
  		if(tmp[0] == '/') tmp++;
  		tmp2 = tmp + 6;
  		if(tmp2[0] == '/') tmp2 ++;
  		
  		// Get a given agent
  		if(strlen(tmp2) > 0){
				ag = agent_get(tmp2);
				if(ag == NULL) fail = AGENT_NOT_FOUND;
				else	{
					shell_output_str(&emma_shell_get_command, "Name: ", ag->name);		
					shell_output_str(&emma_shell_get_command, "Pre: ", ag->pre);		
					shell_output_str(&emma_shell_get_command, "Target: ", ag->target);		
					shell_output_str(&emma_shell_get_command, "Post: ", ag->post);		
					}
  			}
  		// Get the agents list
  		else {
  			for (	ag = (agent_t*)list_head(((list_t)agent_get_all())); 
							ag; 
							ag = ag->next)
					shell_output_str(&emma_shell_get_command, "", ag->name);
								
  			}
  		}
  	// Get the resources list
  	else if(tmp[0] == '/'){
  		for (		r = (resource_t*)list_head(rest_get_resources()); 
  						r; r = r->next)
  				shell_output_str(&emma_shell_get_command, "", r->url);
  				
  		}
  	// Get a given resource
  	else {
	  	r = rest_get_resource(tmp);
	  	if(r == NULL) 	fail = RESOURCE_NOT_FOUND;
	  	else {
	  		if(r->pre_handler) r->pre_handler(NULL, NULL);
	  	 	tmp  = rest_get_user_data(r);
	  		shell_output_str(&emma_shell_get_command, "", tmp?tmp:"");
				}
			}
  	}
	emma_shell_print_res(fail, &emma_shell_get_command, &emma_shell_get_usage);
}
/***********************************************************/
PROCESS_THREAD(emma_shell_get_process, ev, data){
  PROCESS_BEGIN();
  
  if(strlen(data) > 0)
  	emma_shell_get(data);
  else {
    PROCESS_WAIT_UNTIL(ev == shell_event_input);
    emma_shell_get((char*)((struct shell_input*)data)->data2);
		}
  PROCESS_END();
}
/***********************************************************/
/***********************************************************/
/* PUT */
/***********************************************************/
PROCESS(emma_shell_put_process, "eput");
SHELL_COMMAND(emma_shell_put_command,
	      "eput",
	      "eput: set ressource value (eput uri value)",
	      &emma_shell_put_process);
/***********************************************************/
void 
emma_shell_put_usage(){
  shell_output_str(&emma_shell_put_command, "USAGE: ", "eput uri value");
}
/***********************************************************/
void
emma_shell_put(char* cmd){
  resource_t * r = NULL;
  char fail = 0;

  char * tmp = strtok(cmd," ");
	if(!fail && tmp == NULL) fail = EMPTY_ARGUMENT;
	
	if(!fail) 	r = rest_get_resource(tmp);
  if(!fail && r == NULL) 	fail = RESOURCE_NOT_FOUND;
  
  if(!fail)	  tmp = strtok(NULL," ");
  if(!fail && tmp == NULL) fail = EMPTY_ARGUMENT;
  
  if(!fail) sprintf(r->user_data,"%d",vm_eval(tmp));		
	emma_shell_print_res(fail, &emma_shell_put_command, &emma_shell_put_usage);
}
/***********************************************************/
PROCESS_THREAD(emma_shell_put_process, ev, data){
  PROCESS_BEGIN();
  
  if(strlen(data) > 0)
  	emma_shell_put(data);
  else {
    PROCESS_WAIT_UNTIL(ev == shell_event_input);
    emma_shell_put((char*)((struct shell_input*)data)->data2);
		}
  PROCESS_END();
}
/***********************************************************/
/***********************************************************/
/* POST */
/***********************************************************/
PROCESS(emma_shell_post_process, "epost");
SHELL_COMMAND(emma_shell_post_command,
	      "epost",
	      "epost: create ressource/agent (epost uri object)",
	      &emma_shell_post_process);
/***********************************************************/
void 
emma_shell_post_usage(){
  shell_output_str(&emma_shell_post_command, "USAGE: ", "epost uri object");
}
/***********************************************************/
void
emma_shell_post(char* resource){
	char name[AGENT_NAME_SIZE];
	char pre[AGENT_PRE_SIZE];
	char target[AGENT_TARGET_SIZE];
	char post[AGENT_POST_SIZE];
  char fail = 0;

  char * tmp = strtok(resource," ");
	if(!fail && tmp == NULL) fail = EMPTY_ARGUMENT;
	
	if(!fail) {
		sprintf(name, "%s", tmp);
		char* tmp2 = strstr(name,"agents/");
		if(tmp2 != NULL) {
			tmp2 += 7;
			tmp = strtok(NULL," ");
			sprintf(pre, "%s", tmp?tmp:"");
			tmp = strtok(NULL," ");
			sprintf(target, "%s", tmp?tmp:"");
			tmp = strtok(NULL," ");
			
			// No agent name
			if(strlen(tmp2)==0)
						fail = EMPTY_ARGUMENT;
			else 	fail = agent_new(tmp2, pre, target, tmp?tmp:"");
			}
		else{
			tmp = strtok(NULL," ");
			if(strcmp(name,"agents") == 0) fail = RESOURCE_ALREADY_EXIST;
			else fail = rest_add_resource (name, tmp);
			}
  	}
	emma_shell_print_res(fail, &emma_shell_post_command, &emma_shell_post_usage);
}
/***********************************************************/

PROCESS_THREAD(emma_shell_post_process, ev, data){
  PROCESS_BEGIN();
  
  if(strlen(data) > 0)
  	emma_shell_post(data);
  else {
    PROCESS_WAIT_UNTIL(ev == shell_event_input);
    emma_shell_post((char*)((struct shell_input*)data)->data2);
		}
  PROCESS_END();
}
/***********************************************************/
/***********************************************************/
/* DELETE */
/***********************************************************/
PROCESS(emma_shell_delete_process, "edelete");
SHELL_COMMAND(emma_shell_delete_command,
	      "edelete",
	      "edelete: delete ressource/agent (edelete uri)",
	      &emma_shell_delete_process);
/***********************************************************/
void 
emma_shell_delete_usage(){
  shell_output_str(&emma_shell_delete_command, "USAGE: ", "edelete uri");
}
/***********************************************************/
void
emma_shell_delete(char* resource){
	emma_error_t fail = 0;
  char * tmp = strtok(resource," ");
	char * tmp2 = strstr(tmp, "agents/");

	// DELETE agent
	if(tmp2 != NULL){
		tmp2 += 7;
		fail = agent_delete(tmp2); 
		}
	else fail = rest_del_resource (tmp);    
	emma_shell_print_res(fail, &emma_shell_delete_command, &emma_shell_delete_usage);
}
/***********************************************************/
PROCESS_THREAD(emma_shell_delete_process, ev, data){
  PROCESS_BEGIN();
  
  if(strlen(data) > 0)
  	emma_shell_delete(data);
  else {
    PROCESS_WAIT_UNTIL(ev == shell_event_input);
    emma_shell_delete((char*)((struct shell_input*)data)->data2);
		}
  PROCESS_END();
}
/***********************************************************/
/***********************************************************/
/* RUN */
/***********************************************************/
PROCESS(emma_shell_run_process, "erun");
SHELL_COMMAND(emma_shell_run_command,
	      "erun",
	      "erun: run agent(s) (erun agent)",
	      &emma_shell_run_process);
/***********************************************************/
void 
emma_shell_run_usage(){
  shell_output_str(&emma_shell_run_command, "USAGE: ", "erun agent");
}
/***********************************************************/
void 
emma_shell_run(char* agent){
	emma_error_t fail = 0;
  agent_t* ag = NULL;
  char * tmp 		= strtok(agent," ");
  
  // Run a given agents
  if(tmp){
	  if(!strncmp(tmp,"/agents",7) || !strncmp(tmp,"agents",6)){
	  	tmp += 7;
	  	if(tmp[0] == '/') tmp++;
	  	ag = agent_get(tmp);
	  	if(ag == NULL) fail = AGENT_NOT_FOUND;
	  	else agent_run(ag);
	  	}
		else fail = AGENT_NOT_FOUND;
	  }
  // Run all agents
  else agent_run_all();

	emma_shell_print_res(fail, &emma_shell_run_command, &emma_shell_run_usage);
}
/***********************************************************/
PROCESS_THREAD(emma_shell_run_process, ev, data){
  PROCESS_BEGIN();
  
  if(strlen(data) > 0)
  	emma_shell_run(data);
  else {
    PROCESS_WAIT_UNTIL(ev == shell_event_input);
    emma_shell_run((char*)((struct shell_input*)data)->data2);
		}
  PROCESS_END();
}
/***********************************************************/

/***********************************************************/
/* EVAL */
/***********************************************************/
PROCESS(emma_shell_eval_process, "eeval");
SHELL_COMMAND(emma_shell_eval_command,
	      "eeval",
	      "eeval: evaluate a given algebra formula (eeval formula)",
	      &emma_shell_eval_process);
/***********************************************************/
void 
emma_shell_eval_usage(){
  shell_output_str(&emma_shell_eval_command, "USAGE: ", "eeval formula");
}
/***********************************************************/
void
emma_shell_eval(char* formula){
	emma_error_t fail = 0;
	int res = 0;
  char * tmp = NULL;
  char buff[BUFF_SIZE];
    
  tmp = strtok(formula," ");
	if(tmp){
		res = vm_eval(tmp);
		sprintf(buff, "%d", res);
		shell_output_str(&emma_shell_eval_command, "", buff);
		}
	else fail = EMPTY_ARGUMENT;

	emma_shell_print_res(fail, &emma_shell_eval_command, &emma_shell_eval_usage);
}
/***********************************************************/
PROCESS_THREAD(emma_shell_eval_process, ev, data){
  PROCESS_BEGIN();
  
  if(strlen(data) > 0)
  	emma_shell_eval(data);
  else {
    PROCESS_WAIT_UNTIL(ev == shell_event_input);
    emma_shell_eval((char*)((struct shell_input*)data)->data2);
		}
  PROCESS_END();
}
/***********************************************************/

/***********************************************************/
/* PARSE */
/***********************************************************/
PROCESS(emma_shell_parse_process, "eparse");
SHELL_COMMAND(emma_shell_parse_command,
	      "eparse",
	      "eparse: parse a given algebra formula by replacing local varaibles (eparse formula)",
	      &emma_shell_parse_process);
/***********************************************************/
void 
emma_shell_parse_usage(){
  shell_output_str(&emma_shell_parse_command, "USAGE: ", "eparse formula");
	}
/***********************************************************/	
void 
emma_shell_parse(char * formula){
	emma_error_t fail = 0;
	char res[BUFF_SIZE];
  char * tmp = NULL;
  
	tmp = strtok(formula," ");
	if(tmp){
		vm_eval_parse(tmp, res);
		shell_output_str(&emma_shell_parse_command, "", res);
		}
	else fail = EMPTY_ARGUMENT;
	emma_shell_print_res(fail, &emma_shell_parse_command, &emma_shell_parse_usage);
	}
/***********************************************************/
PROCESS_THREAD(emma_shell_parse_process, ev, data){
  PROCESS_BEGIN();
  
  if(strlen(data) > 0)
  	emma_shell_parse(data);
  else {
    PROCESS_WAIT_UNTIL(ev == shell_event_input);
    emma_shell_parse((char*)((struct shell_input*)data)->data2);
		}
  PROCESS_END();
	}
/***********************************************************/

/***********************************************************/
/* SEND */
/***********************************************************/
PROCESS(emma_shell_send_process, "esend");
SHELL_COMMAND(emma_shell_send_command,
	      "esend",
	      "esend: send a coap request to a remote node",
	      &emma_shell_send_process);
process_event_t emma_shell_send_event;
/***********************************************************/
void 
emma_shell_send_usage(){
  shell_output_str(&emma_shell_send_command, "USAGE: ", "esend url [method] [body]");
	}
/***********************************************************/	
void
emma_shell_send_callback(error_t err, emma_request_t* req, char* rsp){
	if(rsp != NULL) 
		 shell_output_str(&emma_shell_send_command, rsp, "");
	
	emma_shell_print_res(err, &emma_shell_send_command, NULL);
	
	process_post 	(&emma_shell_send_process, emma_shell_send_event, NULL);

}
/***********************************************************/	
void 
emma_shell_send(char* cmd){
  char * tmp = NULL;
  char url[EMMA_URL_SIZE_MAX];
  method_t method = GET;
  char body[MAX_PAYLOAD_LEN];
  
	tmp = strtok(cmd," ");
	if(tmp == NULL){
		emma_shell_print_res(EMPTY_ARGUMENT, &emma_shell_send_command, &emma_shell_send_usage);
		return;
		}
	sprintf(url, "%s", tmp);
	
	tmp = strtok(NULL," ");
	if(tmp != NULL){
		if(!strcmp(tmp, "GET")) 					method = GET;
		else if(!strcmp(tmp, "PUT")) 			method = PUT;
		else if(!strcmp(tmp, "POST")) 		method = POST;
		else if(!strcmp(tmp, "DELETE")) 	method = DELETE;
		}
		
	tmp = strtok(NULL," ");
	sprintf(body, "%s", tmp?tmp:"");
	
	PRINTF("[SHELL] esend to:\nURL:%s\nMethod:%d\nBody:%s\n", url, method, body);	
	EMMA_SEND(url, emma_shell_send_callback, method, body);
	}
/***********************************************************/
PROCESS_THREAD(emma_shell_send_process, ev, data){
  PROCESS_BEGIN();
  
  if(strlen(data) > 0)
  	emma_shell_send(data);
  else {
    PROCESS_WAIT_UNTIL(ev == shell_event_input);
    emma_shell_send((char*)((struct shell_input*)data)->data2);
		}
	
	PROCESS_WAIT_UNTIL(ev == emma_shell_send_event);	
		
  PROCESS_END();
	}
/***********************************************************/
/***********************************************************/
/* NET-INFO */
/***********************************************************/
PROCESS(emma_shell_netinfo_process, "netinfo");
SHELL_COMMAND(emma_shell_netinfo_command,
	      "netinfo",
	      "netinfo: get network information",
	      &emma_shell_netinfo_process);
/***********************************************************/
void 
emma_shell_netinfo_usage(){
  shell_output_str(&emma_shell_netinfo_command, "USAGE: ", "netinfo");
}
/***********************************************************/
PROCESS_THREAD(emma_shell_netinfo_process, ev, data){
  PROCESS_BEGIN();
  char buf[BUFF_SIZE];
	extern uip_ds6_nbr_t uip_ds6_nbr_cache[];
	extern uip_ds6_route_t uip_ds6_routing_table[];
	extern uip_ds6_netif_t uip_ds6_if;

  uint8_t i,j;
  sprintf(buf, "Addresses [%u max]",UIP_DS6_ADDR_NB);
	shell_output_str(&emma_shell_netinfo_command, buf, "");
  for (i=0;i<UIP_DS6_ADDR_NB;i++) 
    if (uip_ds6_if.addr_list[i].isused) {
      sprintf(buf, " %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", 
      	((u8_t *)&uip_ds6_if.addr_list[i].ipaddr)[0],       	((u8_t *)&uip_ds6_if.addr_list[i].ipaddr)[1], 
      	((u8_t *)&uip_ds6_if.addr_list[i].ipaddr)[2],       	((u8_t *)&uip_ds6_if.addr_list[i].ipaddr)[3], 
      	((u8_t *)&uip_ds6_if.addr_list[i].ipaddr)[4],       	((u8_t *)&uip_ds6_if.addr_list[i].ipaddr)[5], 
      	((u8_t *)&uip_ds6_if.addr_list[i].ipaddr)[6],       	((u8_t *)&uip_ds6_if.addr_list[i].ipaddr)[7], 
      	((u8_t *)&uip_ds6_if.addr_list[i].ipaddr)[8],       	((u8_t *)&uip_ds6_if.addr_list[i].ipaddr)[9], 
      	((u8_t *)&uip_ds6_if.addr_list[i].ipaddr)[10],       	((u8_t *)&uip_ds6_if.addr_list[i].ipaddr)[11], 
      	((u8_t *)&uip_ds6_if.addr_list[i].ipaddr)[12],       	((u8_t *)&uip_ds6_if.addr_list[i].ipaddr)[13], 
      	((u8_t *)&uip_ds6_if.addr_list[i].ipaddr)[14],       	((u8_t *)&uip_ds6_if.addr_list[i].ipaddr)[15]);
      
		shell_output_str(&emma_shell_netinfo_command, buf, "");
    }
 
  sprintf(buf, "\nNeighbors [%u max]",UIP_DS6_NBR_NB);
	shell_output_str(&emma_shell_netinfo_command, buf, "");
  for(i = 0,j=1; i < UIP_DS6_NBR_NB; i++) 
    if(uip_ds6_nbr_cache[i].isused) {
      sprintf(buf, " %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", 
      	((u8_t *)&uip_ds6_nbr_cache[i].ipaddr)[0],      	((u8_t *)&uip_ds6_nbr_cache[i].ipaddr)[1], 
      	((u8_t *)&uip_ds6_nbr_cache[i].ipaddr)[2],       	((u8_t *)&uip_ds6_nbr_cache[i].ipaddr)[3], 
      	((u8_t *)&uip_ds6_nbr_cache[i].ipaddr)[4],       	((u8_t *)&uip_ds6_nbr_cache[i].ipaddr)[5], 
      	((u8_t *)&uip_ds6_nbr_cache[i].ipaddr)[6],       	((u8_t *)&uip_ds6_nbr_cache[i].ipaddr)[7], 
      	((u8_t *)&uip_ds6_nbr_cache[i].ipaddr)[8],       	((u8_t *)&uip_ds6_nbr_cache[i].ipaddr)[9], 
      	((u8_t *)&uip_ds6_nbr_cache[i].ipaddr)[10],      	((u8_t *)&uip_ds6_nbr_cache[i].ipaddr)[11], 
      	((u8_t *)&uip_ds6_nbr_cache[i].ipaddr)[12],      	((u8_t *)&uip_ds6_nbr_cache[i].ipaddr)[13], 
      	((u8_t *)&uip_ds6_nbr_cache[i].ipaddr)[14],      	((u8_t *)&uip_ds6_nbr_cache[i].ipaddr)[15]);
			shell_output_str(&emma_shell_netinfo_command, buf, "");
			j=0;
    	}
  if (j) shell_output_str(&emma_shell_netinfo_command, "<NONE>", "");
  
  sprintf(buf, "\nRoutes [%u max]",UIP_DS6_ROUTE_NB);
	shell_output_str(&emma_shell_netinfo_command, buf, "");
  for(i = 0,j=1; i < UIP_DS6_ROUTE_NB; i++) 
    if(uip_ds6_routing_table[i].isused) {
      sprintf(buf, " %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", 
      	((u8_t *)&uip_ds6_routing_table[i].ipaddr)[0],      	((u8_t *)&uip_ds6_routing_table[i].ipaddr)[1], 
      	((u8_t *)&uip_ds6_routing_table[i].ipaddr)[2],       	((u8_t *)&uip_ds6_routing_table[i].ipaddr)[3], 
      	((u8_t *)&uip_ds6_routing_table[i].ipaddr)[4],       	((u8_t *)&uip_ds6_routing_table[i].ipaddr)[5], 
      	((u8_t *)&uip_ds6_routing_table[i].ipaddr)[6],       	((u8_t *)&uip_ds6_routing_table[i].ipaddr)[7], 
      	((u8_t *)&uip_ds6_routing_table[i].ipaddr)[8],       	((u8_t *)&uip_ds6_routing_table[i].ipaddr)[9], 
      	((u8_t *)&uip_ds6_routing_table[i].ipaddr)[10],      	((u8_t *)&uip_ds6_routing_table[i].ipaddr)[11], 
      	((u8_t *)&uip_ds6_routing_table[i].ipaddr)[12],      	((u8_t *)&uip_ds6_routing_table[i].ipaddr)[13], 
      	((u8_t *)&uip_ds6_routing_table[i].ipaddr)[14],     	((u8_t *)&uip_ds6_routing_table[i].ipaddr)[15]);
			shell_output_str(&emma_shell_netinfo_command, buf, "");
			
  		sprintf(buf, "/%u (via ", uip_ds6_routing_table[i].length);
      shell_output_str(&emma_shell_netinfo_command, buf, "");
      
			sprintf(buf, " %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", 
      	((u8_t *)&uip_ds6_routing_table[i].nexthop)[0],      	((u8_t *)&uip_ds6_routing_table[i].nexthop)[1], 
      	((u8_t *)&uip_ds6_routing_table[i].nexthop)[2],       ((u8_t *)&uip_ds6_routing_table[i].nexthop)[3], 
      	((u8_t *)&uip_ds6_routing_table[i].nexthop)[4],       ((u8_t *)&uip_ds6_routing_table[i].nexthop)[5], 
      	((u8_t *)&uip_ds6_routing_table[i].nexthop)[6],       ((u8_t *)&uip_ds6_routing_table[i].nexthop)[7], 
      	((u8_t *)&uip_ds6_routing_table[i].nexthop)[8],       ((u8_t *)&uip_ds6_routing_table[i].nexthop)[9], 
      	((u8_t *)&uip_ds6_routing_table[i].nexthop)[10],      ((u8_t *)&uip_ds6_routing_table[i].nexthop)[11], 
      	((u8_t *)&uip_ds6_routing_table[i].nexthop)[12],      ((u8_t *)&uip_ds6_routing_table[i].nexthop)[13], 
      	((u8_t *)&uip_ds6_routing_table[i].nexthop)[14],     	((u8_t *)&uip_ds6_routing_table[i].nexthop)[15]);
      shell_output_str(&emma_shell_netinfo_command, buf, "");
      
  		sprintf(buf, ") %lus\n", uip_ds6_routing_table[i].state.lifetime);
      shell_output_str(&emma_shell_netinfo_command, buf, "");
      	
      j=0;
    }
  if (j) shell_output_str(&emma_shell_netinfo_command, "<NONE>", "");
  
  PROCESS_END();
}
/***********************************************************/
void
emma_shell_init(void){
  serial_line_init();
  serial_shell_init();
  
  shell_reboot_init();
  shell_power_init();
  shell_ps_init();
  shell_ping_init();
  
//  shell_coffee_init();
/*
  shell_base64_init();
  shell_blink_init();
// 
  shell_netfile_init();
//  shell_profile_init();
//  shell_rsh_init();
  shell_run_init();
  shell_text_init();
  shell_time_init();
  shell_vars_init();*/
//  shell_udpsend_init();
  
  shell_register_command(&emma_shell_get_command);
  shell_register_command(&emma_shell_put_command);
  shell_register_command(&emma_shell_post_command);
  shell_register_command(&emma_shell_delete_command);
  shell_register_command(&emma_shell_run_command);
  shell_register_command(&emma_shell_eval_command);
  shell_register_command(&emma_shell_parse_command);
  shell_register_command(&emma_shell_send_command);
  shell_register_command(&emma_shell_netinfo_command);
}

#endif
