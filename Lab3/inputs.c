#include <lpc17xx.h> 
#include "inputs.h"


void input_init(void) 
{
    // cofig INT0 button for input
    LPC_PINCON->PINSEL4 &= ~(3<<20); // P2.10 is GPIO
    LPC_GPIO2->FIODIR &= ~(1<<10);   // P2.10 is input
}

void joystick_init(void)
{
    // this step is actually not required normally since they are by default are set to GPIO
    LPC_PINCON->PINSEL3 &= ~((3<< 8) | (3<<14) | (3<<16) | (3<<18) | (3<<20));     /* P1.20, P1.23..26 is GPIO (Joystick) */

    // set the I/O direction; There are five LPC_GPIOx, where x=0,1,2,3,4 
    // To set pin as input, set the corresponding bit in FIODIR to 0, for output, set FIODIR to 1
    // By default all I/Os are input
    LPC_GPIO1->FIODIR &= ~((1<<20) | (1<<23) | (1<<24) | (1<<25) | (1<<26));     /* P1.20, P1.23..26 is input */
}

unsigned char read_joystick(void)
{
    char kbd_val = ~(LPC_GPIO1->FIOPIN >> 20) & 0x79; // 0x79 === 0111 1001
    return kbd_val;
}

void led_init(void) 
{
    // configure LEDs 
    LPC_GPIO1->FIODIR |= 0xB0000000; // LEDs on PORT1 P1.28..29, P1.31 is output  // 1011 0000 0000 0000 0000 0000 0000 0000
    LPC_GPIO2->FIODIR |= 0x0000007C; // LEDs on PORT2 P2.2..6 is output           // 0000 0000 0000 0000 0000 0000 0111 1100
}

void potentiometer_init(void)
{
    LPC_SC->PCONP |= (1 << 12);      // enable power to ADC
    
    LPC_PINCON->PINSEL1 &= ~(3<<18); // clear bits
    LPC_PINCON->PINSEL1 |= (1<<18);  // set bits P0.25 is AD0.2
    
    LPC_ADC->ADCR =  ( 1 <<  2 ) |   // select the second channel
                     ( 4 <<  8 ) |   // ADC clock = 25 MHz / 5
                     ( 1 << 21 );    // enable ADC
    
    LPC_ADC->ADINTEN = ( 1 << 8 );   // enable interrupt for all ADC channels
    
    //NVIC_EnableIRQ(ADC_IRQn);      // register interrupt
}

unsigned char debounce(void) 
{
    // debounce: read the value 3 times in a row; iff the 3 values align, register as input
    char debounce_frame[3] = {0};
    char kbd_val; 
    debounce_frame[0] = read_joystick();
    debounce_frame[1] = read_joystick();
    debounce_frame[2] = read_joystick();
    
    if ((debounce_frame[0] == 0x08 && debounce_frame[1] == 0x08 && debounce_frame[2] == 0x08)
        || (debounce_frame[0] == 0x20 && debounce_frame[1] == 0x20 && debounce_frame[2] == 0x20))
    {
        kbd_val = debounce_frame[2];
    }
    return kbd_val;    
}

