
#include "emma-rest.h"
#define RESSOURCE_MAX			3

void 
resources_init();

resource_t * 
rest_get_resource(char * name);

emma_error_t 
rest_add_resource (char* name, char* data);

emma_error_t
rest_del_resource (char* name);
