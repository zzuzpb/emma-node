#include "emma-resources.h"
#include "rest.h"

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

//MEMB(resources_R, resource_t, RESSOURCE_MAX);
static char 				resources_R_memb_count[RESSOURCE_MAX];
static resource_t 	resources_R_memb_mem[RESSOURCE_MAX];
static struct memb 	resources_R = {sizeof(resource_t), RESSOURCE_MAX, resources_R_memb_count, (void *)resources_R_memb_mem};



resource_t * rest_get_resource(char* name){
  resource_t* resource = NULL;
  for (resource = (resource_t*)list_head(rest_get_resources()); resource; resource = resource->next){
		if(strcmp(resource->url,name) == 0)
			break;
  	}
	return resource;
	}


emma_error_t
rest_add_resource (char* name, char* data){
  resource_t * r = rest_get_resource(name);
		
	if (r != NULL) 
		return RESOURCE_ALREADY_EXIST;
		
	r = memb_alloc(&resources_R);
	if(r == NULL) {
		PRINTF("[RESOURCES] : [ERROR] no resource space available");
		return -1;
		}
		
		sprintf(r->url, "%s", name);

		r->user_data = malloc(BUFF_SIZE * sizeof(char));
		sprintf(r->user_data, "%s", data != NULL ? data : "0");

		rest_activate_resource(r);
		
		return 0;
}

emma_error_t
rest_del_resource (char* name){
  resource_t* r = r = rest_get_resource(name);
  if(r== NULL)
  	return RESOURCE_NOT_FOUND;

	rest_disable_resource(r);
	memb_free(&resources_R, r);
	return 0;
}

void
resources_init(){
	
	memb_init(&resources_R);
}
