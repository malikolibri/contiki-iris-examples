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

unsigned short x = 0;

PROCESS_THREAD(blink_LED, ev, data) {
	static struct etimer et;
	

	PROCESS_BEGIN();
	
	while(1) {
	leds_off(LEDS_ALL);
	printf("Led broj: %i \n",x);
	switch(x){
		case 0: 	
		leds_on(LEDS_YELLOW);
		break;
	
		case 1: 	
		leds_on(LEDS_GREEN);
		break;

		case 2: 	
		leds_on(LEDS_RED);
		break;
	}
	
    x++ ;
    if (x == 3) { x = 0; }
    
    etimer_set(&et, CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));    
 
  	}

  	PROCESS_END();


}


