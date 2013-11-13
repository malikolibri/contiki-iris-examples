#ifndef __MDA100CB_H__
#define __MDA100CB_H__

#include "avr/io.h"
#include "contiki-conf.h"
#include "dev/adc.h"

/* MDA100CB, the light sensor power is controlled
 * by setting signal INT1(PORTD pin 1).
 * Both light and thermistor use the same ADC channel.
 */
#define LIGHT_PORT_DDR DDRE
#define LIGHT_PORT PORTE
#define LIGHT_PIN_MASK _BV(5)
#define LIGHT_ADC_CHANNEL 1

/* MDA100CB, the thermistor power is controlled
 * by setting signal PW0(PORTC pin 0).
 * Both light and thermistor use the same ADC channel.
 */
#define TEMP_PORT_DDR DDRC
#define TEMP_PORT PORTC
#define TEMP_PIN_MASK _BV(0)
#define TEMP_ADC_CHANNEL 1


uint16_t get_light();
uint16_t get_temp();

#endif //__MDA100CB_H__
