#include "emma-rest.h"
#include "emma-rest-client.h"
#include "emma-agent.h"
#include "emma-vm.h"
#include "memb.h"
#include <string.h>

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

LIST(agents);
MEMB(agents_A, agent_t, AGENT_MAX);
/***********************************************************/
/* Agent VM */
/***********************************************************/
void 
agent_run(agent_t* ag){
	char res[BUFF_SIZE];
	PRINTF("[AGENT] run %s\n",ag->name);
	
	if(vm_eval(ag->pre)){
		strcpy(res,"&value=");
		vm_eval_parse(ag->post, res+7);
		PRINTF("[AGENT] %s post %s\n",ag->name, res);
		emma_send(ag->target, NULL, PUT, res);
		}	
	}

void
agent_run_all(){
	agent_t* ag = NULL;
	
	for (ag = (agent_t*)list_head(((list_t)agent_get_all())); 
		ag; 
		ag = ag->next)
		agent_run(ag);
	}

/***********************************************************/
/* Agent manager */
/***********************************************************/
int 
agent_new(char * name, char * pre, char* target, char * post){
	if( strlen(name) > AGENT_NAME_SIZE ||
			strlen(pre)  > AGENT_PRE_SIZE ||
			strlen(post) > AGENT_POST_SIZE){
			PRINTF("[AGENT] [ERROR] invalid agent");
			return EMPTY_ARGUMENT;
			}

	agent_t* ag = memb_alloc(&agents_A);
	if(ag == NULL) {
		PRINTF("[AGENT] [ERROR] no agent space available");
		return NOT_ENOUGH_MEMORY;
		}
	strcpy(ag->name,name);
	strcpy(ag->pre,pre);
	strcpy(ag->target, target);
	strcpy(ag->post,post);
	list_add(agents, ag);
	return 0;
	}
	
agent_t* 
agent_get(char * name){
  agent_t* agent = NULL;
  for (		agent = (agent_t*)list_head(agents); 
  				agent; 
  				agent = agent->next)
  	if(strcmp(agent->name,name) == 0) 
  		return agent;
		
	return NULL;
	}
	
agent_t* agent_get_all(void){
	return ((agent_t*)agents);
	}
	
int 
agent_delete(char* name){
	agent_t* ag = agent_get(name);
	if(ag == NULL){
		PRINTF("AGENT [ERROR] unknown agent %s", name);
		return AGENT_NOT_FOUND;
		}
	list_remove(agents, ag);
	memb_free(&agents_A, ag);
	return 0;
	}
/***********************************************************/
/* Agent system */
/***********************************************************/
void 
agent_init(void){
  list_init(agents);
	memb_init(&agents_A);
	}
