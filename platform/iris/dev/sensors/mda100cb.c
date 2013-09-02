#include "mda100cb.h"

void
adc_init()
{
  ADMUX = 0;
  /* AVCC with external capacitor at AREF pin. */
  //ADMUX |= _BV(REFS0)
  /* Disable ADC interrupts. */
  ADCSRA &= ~( _BV(ADIE) | _BV(ADIF) );
  /* Set ADC prescaler to 64 and clear interrupt flag. */
  ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADIE);

}
/*---------------------------------------------------------------------------*/
/* Poll based approach. The interrupt based adc is currently not used.
   The ADC result is right adjusted. First 8 bits(from left) are in ADCL and
   other two bits are in ADCH. See Atmega128 datasheet page 228. */
uint16_t
get_adc(int channel)
{
  uint16_t reading;

  ADMUX |= (channel & 0x1F);

  /* Disable ADC interrupts. */
  ADCSRA &= ~_BV(ADIE);
  /* Clear previous interrupts. */
  ADCSRA |= _BV(ADIF);
  /* Enable ADC and start conversion. */
  ADCSRA |= _BV(ADEN) | _BV(ADSC);
  /* Wait until conversion is completed. */
  while ( ADCSRA & _BV(ADSC) );
  /* Get first 8 bits.  */
  reading = ADCL;
  /* Get last two bits. */
  reading |= (ADCH & 3) << 8;
  /* Disable ADC. */
  ADCSRA &= ~_BV(ADEN);
  return reading;
}
/*---------------------------------------------------------------------------*/
uint16_t
get_light()
{
  uint16_t reading;

  /* Enable light sensor. */
  LIGHT_PORT |= LIGHT_PIN_MASK;
  LIGHT_PORT_DDR |= LIGHT_PIN_MASK;
  /* Disable temperature sensor. */
  TEMP_PORT_DDR &= ~TEMP_PIN_MASK;
  TEMP_PORT &= ~TEMP_PIN_MASK;
  /* Read ADC. */
  reading = get_adc(LIGHT_ADC_CHANNEL);
  /* Disable light sensor. */
  LIGHT_PORT &= ~LIGHT_PIN_MASK;
  LIGHT_PORT_DDR &= ~LIGHT_PIN_MASK;
    return reading;
}
/*---------------------------------------------------------------------------*/
uint16_t
get_temp()
{
  uint16_t reading;

  /* Disable light sensor. */
  LIGHT_PORT &= ~LIGHT_PIN_MASK;
  LIGHT_PORT_DDR &= ~LIGHT_PIN_MASK;
  /* Enable temperature sensor. */
  TEMP_PORT_DDR |= TEMP_PIN_MASK;
  TEMP_PORT |= TEMP_PIN_MASK;
  /* Read ADC. */
  reading = get_adc(TEMP_ADC_CHANNEL);
  /* Disable temperature sensor. */
  TEMP_PORT_DDR &= ~TEMP_PIN_MASK;
  TEMP_PORT &= ~TEMP_PIN_MASK;

  return reading;
}
/*---------------------------------------------------------------------------*/
