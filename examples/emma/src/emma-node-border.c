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

PROCESS_THREAD(emma_node, ev, data)
{
  PROCESS_BEGIN();
	EMMA_INIT();
  PROCESS_END();
}
