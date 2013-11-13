#include "contiki.h"
#include "stdio.h"
#include "sys/node-id.h"
/*---------------------------------------------------------------------------*/
PROCESS(setID, "Upis node_id");
AUTOSTART_PROCESSES(&setID);
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(setID, ev, data) {

PROCESS_BEGIN();
	watchdog_stop();
	static struct etimer et;

#if NODEID

	printf("Promena node ID-a...\n");
	node_id_burn(NODEID); //za detalje pogledati u "platform/iris/node_id.c"
	node_id_restore();
	printf("Node ID je setovan na: %i \n", node_id);

#else 

	while(1){
		node_id_restore();
		printf("Node ID je setovan na: %i \n", node_id);
		etimer_set(&et, CLOCK_SECOND*2);
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
	}
#endif
PROCESS_END();

}
