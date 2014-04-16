#include "contiki.h"
#include "lib/random.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"
#include "sys/node-id.h"
#include "net/uip.h"
#include "net/uip-ds6.h"
#include "dev/leds.h"
#include "simple-udp.h"
#include <stdio.h>
#include <string.h>

#define UDP_PORT 5555
	
#define SEND_TIME CLOCK_SECOND + (random_rand() % (1 * CLOCK_SECOND))//4-3

static process_event_t podaci_spremni, podaci_poslati;
static struct simple_udp_connection send_sensor_data;

int akum_o;
int akum_t;

uip_ipaddr_t addr;

typedef struct {
	unsigned char color;
	unsigned int time;
} blink_t;


/*---------------------------------------------------------------------------*/
PROCESS(merenje, "merenje osvetljenja");
PROCESS(udp_send_process, "slanje UDP paketa");
PROCESS(blinkLED, "blinkanje LED-a");
AUTOSTART_PROCESSES(&merenje, &udp_send_process, &blinkLED);
/*---------------------------------------------------------------------------*/
/*static void*/
/*receiver(struct simple_udp_connection *c,*/
/*         const uip_ipaddr_t *sender_addr,*/
/*         uint16_t sender_port,*/
/*         const uip_ipaddr_t *receiver_addr,*/
/*         uint16_t receiver_port,*/
/*         const uint8_t *data,*/
/*         uint16_t datalen)*/
/*{*/
/*  printf("Data received on port %d from port %d with length %d\n",*/
/*         receiver_port, sender_port, datalen);*/
/*}*/
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(merenje, ev, data) {
	static struct etimer et1;
	static unsigned short int i;
	static int tmp_o, tmp_t;
	
	PROCESS_BEGIN();
	podaci_spremni = process_alloc_event();
	adc_init();
	
	while(1) {	
		printf("Uzorak-> ");
		tmp_o = 0;
		tmp_t = 0;
		for (i=0; i<4; i++){
			tmp_o += get_light();	
			tmp_t += get_temp();
			//printf("Odbirak temperature: %d i osvetljenja: %d \n",tmp_t,tmp_o);		
			etimer_set(&et1, (CLOCK_SECOND >> 2) );
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et1));			
		}
		akum_o = tmp_o;
		akum_t = tmp_t;
		//printf("Akumulirana vrednost temperature: %d i osvetljenja: %d \n",akum_t,akum_o);
		process_post(&udp_send_process, podaci_spremni, NULL);	
		PROCESS_WAIT_EVENT_UNTIL(ev == podaci_poslati);	   
  	}

  	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_send_process, ev, data )
{
	static struct etimer send_timer;
	static char paket[5];
	static blink_t blinkCrvena;
    

	PROCESS_BEGIN();
	podaci_poslati = process_alloc_event();
	uip_ip6addr(&addr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0x0001); 
	simple_udp_register(&send_sensor_data, UDP_PORT,
		              NULL, UDP_PORT,
		              NULL);
	
	while(1){
		PROCESS_WAIT_EVENT_UNTIL(ev == podaci_spremni);
		paket[0] = (char)(akum_o >> 8);
		paket[1] = (char)(akum_o & 0x00ff);
		paket[2] = (char)(akum_t >> 8);
		paket[3] = (char)(akum_t & 0x00ff);
		paket[4] = (char) node_id;
		etimer_set(&send_timer, SEND_TIME);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&send_timer));
		simple_udp_sendto(&send_sensor_data, &paket[0] , 5, &addr);
		printf("poslat\n");
		blinkCrvena.color = LEDS_RED;
		blinkCrvena.time = (CLOCK_SECOND>>4); //blink na 1 s
		process_post(&blinkLED,PROCESS_EVENT_CONTINUE ,&blinkCrvena);
		process_post(&merenje, podaci_poslati,NULL);
	}
	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blinkLED, ev, data )
{
	static struct etimer blink_timer;
	static blink_t * blink;

	PROCESS_BEGIN();
	while(1){
	    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
	    blink = data;
	    leds_on(blink->color);
	    etimer_set(&blink_timer, (blink->time));
	    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&blink_timer));
	    leds_off(blink->color);
	}
	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
