#include "mda100cb.h"


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
