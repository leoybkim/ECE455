#include <lpc17xx.h>
#include "potentiometer.h"

void init_potentiometer (void)
{
    LPC_PINCON->PINSEL1 &= ~(3<18);            // clear bits
    LPC_PINCON->PINSEL1 |=  (1<<18);
    LPC_SC->PCONP       |=  (1<<12);           // enable power to ADC
    LPC_ADC->ADCR = (1<<2) | (4<<8) | (1<<21); // select the second channel | ADC clock = 25 MHz /5 | enable ADC
}

uint32_t read_potentiometer (void)
{
    while (LPC_ADC->ADGDR & 0x8000 == 0); // wait for conversion
    return (LPC_ADC->ADGDR>>4) & 0xFFF;   // read result
}

void enable_irq_potentiometer (void)
{
    LPC_ADC->ADINTEN = (1<<8);    // enable interrupt for all ADC channels
    NVIC_EnableIRQ(ADC_IRQn);     // register interrupt
}

void potentiometer_conversion(void)
{
    LPC_ADC->ADCR |= (1<<24);     // read the converted value
}
