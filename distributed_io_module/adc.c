/*
 * adc.c
 *
 *  	Created on: Jan 16, 2011
 *      Author: Cristian Axenie
 *      Fisier cu functii de configurare si de preluare a datelor specifice ADC
 */

#include <avr/io.h>
#include <util/delay.h>

// 0x60 = 01100000 
#define ADC_VREF_TYPE 0x60

void init_adc(void);
unsigned char read_adc(unsigned char);

void init_adc(void)
{
  // ADC initialization
  // ADC Clock frequency: 1000.000 kHz
  // ADC Voltage Reference: AVCC pin
  // Only the 8 most significant bits of
  // the AD conversion result are used
  ADMUX=ADC_VREF_TYPE & 0xff;// 01100000 & 11111111 = 01100000
  // 0 1 1 ... AVCC with external capacitor at AREF pin (bits 7 and 6) and ADC Left Adjust Result (bit 5)
  ADCSRA=0x84;// 10000100 --> ADC Enable (bit 7) and Division Factor 16 (bits 2 1 0 combination)
}


// Read the 8 most significant bits (ADCH register)
// of the AD conversion result
unsigned char read_adc(unsigned char adc_input)
{
  ADMUX = adc_input | (ADC_VREF_TYPE & 0xff);
  /* Delay needed for the stabilization of the ADC input voltage*/
  _delay_us(10.0);
  // Start the AD conversion
  ADCSRA|=0x40;// 01000000 --> ADSC bit set = AD start conversion
  // Wait for the AD conversion to complete
  while ((ADCSRA & 0x10)==0);// Test the ADIF bit (AD interrupt flag) for AD conversion finish
  ADCSRA|=0x10;
  return ADCH;// return the conversion result ADCH
}

