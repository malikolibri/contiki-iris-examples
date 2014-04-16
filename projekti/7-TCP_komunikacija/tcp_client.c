#include "contiki.h"
#include "contiki-net.h"
#include "lib/random.h"
#include "sys/ctimer.h"
#include "net/uip.h"
#include "net/uip-ds6.h"
#include "net/uip-udp-packet.h"
#include "sys/ctimer.h"
#include "dev/leds.h"
#include <stdio.h>
#include <string.h>

struct LED_state {
   enum {LED_OFF, LED_ON} state;
};
/*---------------------------------------------------------------------------*/
PROCESS(tcp_led, "TCP client process");
AUTOSTART_PROCESSES(&tcp_led);
/*---------------------------------------------------------------------------*/
void client_init(void) {
   tcp_listen(UIP_HTONS(55555)); //proveriti!
}
/*---------------------------------------------------------------------------*/
void client_app(void) {
   static struct LED_state s;
   char *appdata;
   
   appdata = (char *)uip_appdata;
   if(uip_connected()) {
      leds_off(LEDS_RED);
      s.state = LED_OFF;
      uip_send("Da bi promenili stanje LED posaljite <L>\n", 41);
      return;
   } 


   if(uip_newdata()) {
        if(appdata[0]=='L'){
		  switch(s.state) {
		  case LED_OFF:
		     leds_on(LEDS_RED);
		     s.state = LED_ON;
		     uip_send("LED ukljucen!\n", 14);
		     break;
		  case LED_ON:
		     leds_off(LEDS_RED);
		     s.state = LED_OFF;
		     uip_send("LED iskljucen\n", 14);
		     break;
		  }
	   } else {
         uip_send("Da bi promenili stanje LED posaljite <L>\n", 41);
       }
   }

   if(uip_rexmit()){
      switch(s.state) {
		  case LED_OFF:
		     uip_send("LED iskljucen\n", 14);
		     break;
		  case LED_ON:
             uip_send("LED ukljucen!\n", 14);
		     break;
      }
   }
}
/*---------------------------------------------------------------------------*/
static void
set_global_address(void)
{
  uip_ipaddr_t ipaddr;

  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(tcp_led, ev, data)
{
  PROCESS_BEGIN();

  set_global_address();
  client_init();

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
    client_app();
    
  }
  PROCESS_END();
}