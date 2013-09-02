/*
 *	blink.c - Uzastopno blinkanje LED, svaka po 1s 
 */

#include "contiki.h"
#include "dev/sensors/mts300.h" //drajver za senzorsku plocicu
#include "dev/leds.h"
#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(merenje, "merenje temperature");
PROCESS(ispis, "ispis");
AUTOSTART_PROCESSES(&merenje);
/*---------------------------------------------------------------------------*/



PROCESS_THREAD(merenje, ev, data) {
	static struct etimer et1;
	static unsigned short int i, akum; // promenljive za akumulaciju u iterator
	int  prosek; // promenljive za  prosecnu temperaturu. 
	//U ovom slucaju nema potrebe da se cuvaju izmedju dva budjenja procesa pa zato nema ni "static"
	
	PROCESS_BEGIN();
	
	while(1) {
		leds_off(LEDS_ALL);
		leds_on(LEDS_RED);		
		printf("Merenje \n");
		akum = 0;
		for (i=0; i<3; i++){
			akum += get_temp();		// funkcija je deklarisana u mts300.h Kao izlaz daje sirovu vrednost ADC zakacenog na temp senzor.	
			etimer_set(&et1, CLOCK_SECOND / 3);
			PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
			
		}
		
		prosek = akum / 3;
		process_start(&ispis, prosek);		
		    
  	}

  	PROCESS_END();
}

PROCESS_THREAD(ispis, ev, data) {

	PROCESS_BEGIN();
	

		leds_off(LEDS_ALL);
		leds_on(LEDS_GREEN);
		printf("Trenutna temperatura je: %i \n", data);		



  	PROCESS_END();
}
