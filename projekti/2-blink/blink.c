/*
 *	blink.c - Primer sa vise procesa
 */

#include "contiki.h"
#include "dev/leds.h"

/*---------------------------------------------------------------------------*/
PROCESS(blink_Z, "zeleno");
PROCESS(blink_C, "crveno");
AUTOSTART_PROCESSES(&blink_Z, &blink_C);
/*---------------------------------------------------------------------------*/



PROCESS_THREAD(blink_C, ev, data) {
	static struct etimer et1;
	
	PROCESS_BEGIN();
	
	while(1) {
		leds_toggle(LEDS_RED);	
		etimer_set(&et1, CLOCK_SECOND>>4);
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);    
  	}

  	PROCESS_END();
}

PROCESS_THREAD(blink_Z, ev, data) {
	static struct etimer et2;

	PROCESS_BEGIN();
	
	while(1) {
		leds_toggle(LEDS_GREEN);	
		etimer_set(&et2, CLOCK_SECOND>>2);
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);    
  	}

  	PROCESS_END();
}
