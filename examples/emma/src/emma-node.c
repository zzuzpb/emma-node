#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "emma-rest.h"

#include "emma-rest-client.h"
#include "emma-util.h"

PROCESS(emma_node, "Emma node");
AUTOSTART_PROCESSES(&emma_node);

EMMA_RESSOURCE(energy);

PROCESS_THREAD(emma_node, ev, data)
{
  PROCESS_BEGIN();
	EMMA_INIT();
	EMMA_ACTIVATE(energy, "10");
  PROCESS_END();
}
