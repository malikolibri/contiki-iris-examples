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
static int period = 3;
static int send_time;
static process_event_t podaci_spremni, promenjena_perioda;
static struct simple_udp_connection send_sensor_data;

static int akum_o = 0;
static int akum_t = 0;
static int baterija = 0;

uip_ipaddr_t addr;

typedef struct {
	unsigned char color;
	unsigned int time;
} blink_t;

typedef struct {
    uint8_t * data;
    uint16_t datalen;
} paket_d;

/*---------------------------------------------------------------------------*/
PROCESS(merenje, "merenje osvetljenja");
PROCESS(udp_send_process, "slanje UDP paketa");
PROCESS(udp2children, "Slanje UDP paketa neposrednim potomcima");
PROCESS(blinkLED, "blinkanje LED-a");
AUTOSTART_PROCESSES(&merenje, &udp_send_process, &blinkLED);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp2children, ev , data) {
        
        static char *appdata; //bekap promenljiva
        static uint16_t appdata_len; //bekap
        static uip_ds6_route_t *r; 
        static struct etimer pauza;
        static paket_d * paket;
        
        PROCESS_BEGIN();
        paket = (paket_d *)data;
        appdata_len = paket->datalen;
        appdata = (char *)malloc(appdata_len);
        memcpy(appdata, paket->data, appdata_len);
     
        for(r = uip_ds6_route_head(); r != NULL; r = uip_ds6_route_next(r)) {
          if (((&r->ipaddr)->u16[7]) == ((uip_ds6_route_nexthop(r))->u16[7])){
              
            simple_udp_sendto(&send_sensor_data, appdata , appdata_len,
                            uip_ds6_route_nexthop(r));
            
            etimer_set(&pauza,  CLOCK_SECOND>>2);
            PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
          }
        }
        
        free(appdata);
        
        PROCESS_END();
}
/*---------------------------------------------------------------------------*/
static void 
promene_periode(const uint8_t *data,
         		uint16_t datalen)
{
	static uint8_t period_c[5];
	static uint8_t i, broj_cifara;
	static uint8_t j = 0;
	const uint16_t power[] = {1, 10, 100, 1000, 10000};
	promenjena_perioda = process_alloc_event();
  	for(i = 0; i<datalen;i++){
  		if (data[i] == 'p'){
  			period = 0;
  			while((data[i+1]) > 47 && (data[i+1] < 58)){
  				period_c[j++] = data[++i] - 48;
  			}
  			broj_cifara=j;
  			for(; j>0; j--){
  				period += period_c[j-1] * power[broj_cifara - j];
  			}
  			send_time = random_rand() % (period * CLOCK_SECOND);
  			process_post(&merenje, promenjena_perioda, NULL);
  			process_post(&udp_send_process, promenjena_perioda, NULL);
  		}
  	}	
}
/*---------------------------------------------------------------------------*/
static void
receiver(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         uint8_t *data,
         uint16_t datalen)
{
	static blink_t blinkZelena;
	static paket_d paketDetetu;
    
    paketDetetu.data = data;
    paketDetetu.datalen = datalen;
	process_start(&udp2children,&paketDetetu);
	
  	promene_periode(data, datalen);
  	blinkZelena.color = LEDS_GREEN;
  	blinkZelena.time = (CLOCK_SECOND>>4); 
  	process_post(&blinkLED,PROCESS_EVENT_CONTINUE ,&blinkZelena);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(merenje, ev, data) {
	static struct etimer et1;
	static unsigned short int i;
	static int tmp_o, tmp_t, perioda_semplovanja;
	
	PROCESS_BEGIN();
	
	podaci_spremni = process_alloc_event();
	perioda_semplovanja=((period * CLOCK_SECOND) >> 2);//inicijalno setovanje
	//process_post(&udp_send_process, podaci_spremni, NULL); //inicijalno slanje
	while(1) {	
		printf("Uzorak-> \n");
		tmp_o = 0;
		tmp_t = 0;
		adc_init();
		for (i=0; i<4; i++){
			tmp_o += get_light();	
			tmp_t += get_temp();
			//printf("Odbirak [%d] temperature: %d i osvetljenja: %d \n",i,tmp_t,tmp_o);
			if (ev==promenjena_perioda)	{
				perioda_semplovanja=((period * CLOCK_SECOND) >> 2);
			}	
			etimer_set(&et1, perioda_semplovanja );
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et1)||(ev==promenjena_perioda));			
		}
		adc_init();
		baterija = get_battery();
		
		akum_o = tmp_o;
		akum_t = tmp_t;
		
		process_post(&udp_send_process, podaci_spremni, NULL);	
			   
  	}

  	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_send_process, ev, data )
{
	static struct etimer send_timer;
	static char paket[7];
	static blink_t blinkCrvena;
    

	PROCESS_BEGIN();
	send_time = random_rand() % (period * CLOCK_SECOND);
	//printf("%d --- %d", CLOCK_SECOND, send_time);
	//podaci_poslati = process_alloc_event();
	uip_ip6addr(&addr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0x0001); 
	simple_udp_register(&send_sensor_data, UDP_PORT,
		              NULL, UDP_PORT,
		              receiver);
	
	while(1){
		PROCESS_WAIT_EVENT_UNTIL(ev == podaci_spremni);
		printf("baterija: %d , temperatura: %d, osvetljenje %d \n", baterija, akum_t, akum_o);
		paket[0] = (char) node_id;
		paket[1] = (char)(akum_o >> 8);
		paket[2] = (char)(akum_o & 0x00ff);
		paket[3] = (char)(akum_t >> 8);
		paket[4] = (char)(akum_t & 0x00ff);
		paket[5] = (char)(baterija >> 8);
		paket[6] = (char)(baterija & 0x00ff);

		etimer_set(&send_timer, send_time);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&send_timer)||(ev==promenjena_perioda));
		simple_udp_sendto(&send_sensor_data, &paket[0] , 7, &addr);
		printf("poslat\n");
		blinkCrvena.color = LEDS_RED;
		blinkCrvena.time = (CLOCK_SECOND>>4); 
		process_post(&blinkLED,PROCESS_EVENT_CONTINUE ,&blinkCrvena);
		
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
