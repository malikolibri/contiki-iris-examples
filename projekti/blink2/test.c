/*
 *	blink.c - Uzastopno blinkanje LED, svaka po 1s 
 */

#include "contiki.h"
#include "dev/leds.h"
#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(blink_Z, "zeleno");
PROCESS(blink_C, "crveno");
AUTOSTART_PROCESSES(&blink_Z, &blink_C);
/*---------------------------------------------------------------------------*/



PROCESS_THREAD(blink_C, ev, data) {
	static struct etimer et1;
	
	PROCESS_BEGIN();
	
	while(1) {
		leds_off(LEDS_ALL);
		leds_on(LEDS_RED);
		printf("Crveno \n");		
		etimer_set(&et1, CLOCK_SECOND / 2);
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);    
  	}

  	PROCESS_END();
}

PROCESS_THREAD(blink_Z, ev, data) {
	static struct etimer et2;

	PROCESS_BEGIN();
	
	while(1) {
		leds_off(LEDS_ALL);
		leds_on(LEDS_GREEN);
		printf("Zeleno \n");		
		etimer_set(&et2, CLOCK_SECOND * 2);
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);    
  	}

  	PROCESS_END();
}
