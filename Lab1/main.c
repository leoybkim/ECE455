#include <lpc17xx.h> 
#include "glcd.h"
#include "fsm.h"

/* Thermostat */


// Global variable
const led_pos[8] = { 28, 29, 31, 2, 3, 4, 5, 6 };
int led = 7;


void input_init(void) 
{
    // cofig INT0 button for input
    LPC_PINCON->PINSEL4 &= ~(3<<20); // P2.10 is GPIO
    LPC_GPIO2->FIODIR &= ~(1<<10);  // P2.10 is input
    
    // 0000 0000 0000 0000 0000 0011
    // 3 << 20
    // 1100 0000 0000 0000 0000 0000
    // ~ bitwise NOT operator
    // 0011 1111 1111 1111 1111 1111
    // &= bitwise AND operator
    // enables INT0 button to be used for input by setting bit 21 and 20 of the PINSEL4 register to zero
    // enables GIPO2 I/O direction register to zero 
    // in theory you can omit these two lines since zero is the default value fot both of these registers
}

void joystick_init(void)
{
    // configure joystick
    // this step is actually not required normally since they are by default are set to GPIO
    LPC_PINCON->PINSEL3 &= ~((3<< 8) | (3<<14) | (3<<16) | (3<<18) | (3<<20));     /* P1.20, P1.23..26 is GPIO (Joystick) */

    // set the I/O direction
    // There are five LPC_GPIOx, where x=0,1,2,3,4 
    // To set pin as input, set the corresponding bit in FIODIR to 0, for output, set FIODIR to 1
    // By default all I/Os are input
    LPC_GPIO1->FIODIR &= ~((1<<20) | (1<<23) | (1<<24) | (1<<25) | (1<<26));     /* P1.20, P1.23..26 is input */
}

unsigned char read_joystick(void)
{
    char kbd_val = ~(LPC_GPIO1->FIOPIN >> 20) & 0x79; // 0x79 === 0111 1001
    return kbd_val;
}

unsigned char debounce(void) 
{
    // debounce: read the value 3 times in a row
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

void led_init(void) 
{
    // configure LEDs 
    LPC_GPIO1->FIODIR |= 0xB0000000; // LEDs on PORT1 P1.28..29, P1.31 is output  // 1011 0000 0000 0000 0000 0000 0000 0000
    LPC_GPIO2->FIODIR |= 0X0000007C; // LEDs on PORT2 P2.2..6 is output           // 0000 0000 0000 0000 0000 0000 0111 1100
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

// Turns LED on and off to simulate the furnace
void check_furnace(int desired_temp, int current_temp, int led) 
{
    int mask = 1 << led_pos[led];

    if (led < 3) {
        if (desired_temp > current_temp) {
            LPC_GPIO1->FIOSET = mask;
        } else {
            LPC_GPIO1->FIOCLR = mask;
        }
    } else {
        if (desired_temp > current_temp) {
            LPC_GPIO2->FIOSET = mask;
        } else {
            LPC_GPIO2->FIOCLR = mask;
        }
    } 
}


// timer to wake up from sleep
void timer_init(void)
{
    LPC_TIM0->TCR = 0x02; // reset time
    LPC_TIM0->TCR = 0x01; // enable timer
    LPC_TIM0->MR0 = 4000000; // match value (can be anything)
    LPC_TIM0->MCR |= 0x03; // on match, generate interrupt and reset
    NVIC_EnableIRQ(TIMER0_IRQn); // allow interrupts from the timer
}

void TIMER0_IRQHandler(void)
{
    if((LPC_TIM0->IR & 0x01) == 0x01) // if MR0 interrupt
    {
        LPC_TIM0->IR |= 1 << 0; 
    }
}


// make a generic state machine
// For state: make a table (one axis is previous state, the other axis is the input) and the cell would tell you which state to go to (it could stay in the same state) and only thne
// you would use some sort of an swich case to define an action
// this is so that the user can define what the states are going to be and what actions are going to be 


int main(void)
{
    int joystick_val = 0;
    int int_temperature = 25;
    int ADC_Value = 0;
    unsigned int aDCStat;
    char str_temperature[10];    
    char str_potentiometer[10];
    int i; // for sleep counter
    sprintf(str_temperature, "desired temp: %d", int_temperature);


    SystemInit();   
    GLCD_Init();    
    GLCD_Clear(White);  
    GLCD_DisplayString(0, 0, 1, str_temperature); // line 0, row 0, font 1 (larger of the two available fonts)
      
    // joystick controls the desired temperature
    joystick_init();
    
    // furnace indicator (P2.6)
    led_init();
    
    // change temperature
    potentiometer_init();
    
    // setup timer
    timer_init();
    
    // start program
    for(;;)
    {
        //joystick_val = read_joystick();
        joystick_val = debounce();
        
        if (joystick_val == 0x08) {
            int_temperature += 1;
            sprintf(str_temperature, "desired temp: %d", int_temperature);
            GLCD_DisplayString(0, 0, 1, str_temperature);
            
            /* polling */
            // Read the converted value
            LPC_ADC->ADCR |= ( 1 << 24 );
            // start Conversion
            while (LPC_ADC->ADGDR & 0x8000 == 0);
            ADC_Value = (LPC_ADC->ADGDR>>4) & 0xFFF / 10;
            sprintf(str_potentiometer, "measured temp: %4d", ADC_Value);
            GLCD_DisplayString(1, 0, 1, str_potentiometer);
            
            check_furnace(int_temperature, ADC_Value, led);
            
        } else if (joystick_val == 0x20) {
            int_temperature -= 1;
            sprintf(str_temperature, "desired temp: %d", int_temperature);
            GLCD_DisplayString(0, 0, 1, str_temperature);
            
            /* polling */
            // Read the converted value
            LPC_ADC->ADCR |= ( 1 << 24 );
            // start Conversion
            while (LPC_ADC->ADGDR & 0x8000 == 0);
            ADC_Value = (LPC_ADC->ADGDR>>4) & 0xFFF / 10;
            sprintf(str_potentiometer, "measured temp: %4d", ADC_Value);
            GLCD_DisplayString(1, 0, 1, str_potentiometer);
            
            check_furnace(int_temperature, ADC_Value, led);
        } 
        
        // counter that acts like sleep()
        //for (i=0; i < 1000000; i++) {}
    
        __WFI();    // low power mode until interrupt occurs
    }
    return 0;
}