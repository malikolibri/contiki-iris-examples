/*
 *	blink.c - Uzastopno blinkanje LED, svaka po 1s 
 */

#include "contiki.h"
#include "dev/leds.h"
#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(blink_LED, "trepko");
AUTOSTART_PROCESSES(&blink_LED);
/*---------------------------------------------------------------------------*/



PROCESS_THREAD(blink_LED, ev, data) {
	static struct etimer et;
	static unsigned short x;

	PROCESS_BEGIN();
	x = 0;
	while(1) {
		leds_off(LEDS_ALL);
		printf("Led broj: %i \n",x);
		if (x == 0) {leds_on(LEDS_RED);}
		if (x == 1) {leds_on(LEDS_YELLOW);}
		if (x == 2) {leds_on(LEDS_GREEN);}
		x++ ;
		if (x == 3) { x = 0; }
		
		etimer_set(&et, CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);    
  	}

  	PROCESS_END();
}


