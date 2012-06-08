#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "contiki.h"

#include "emma-rest.h"
#include "emma-rest-client.h"
#include "emma-shell.h"

	
EMMA_RESSOURCE(energy_available);
EMMA_RESSOURCE(energy_need);
EMMA_RESSOURCE(brightness);

PROCESS(emma_node, "Emma node");
AUTOSTART_PROCESSES(&emma_node);

PROCESS_THREAD(emma_node, ev, data)
{
  PROCESS_BEGIN();
  
	EMMA_INIT();
	EMMA_ACTIVATE(brightness, "5");
	EMMA_ACTIVATE(energy_available, "0");
	EMMA_ACTIVATE(energy_need, "1");
	
	#ifdef EMMA_SHELL 	
		emma_shell_init();
	#endif
    
  PROCESS_END();
}
