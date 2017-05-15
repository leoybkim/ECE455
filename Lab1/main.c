#include <lpc17xx.h> // definition for registers on LPC1786
#include "glcd.h" // definition for display routeins that put text and graphics on the LCD


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

void led_init(void) 
{
    // configure LEDs 
    LPC_GPIO1->FIODIR |= 0xB0000000;  // LEDs on PORT1 P1.28..29, P1.31 is output  // 1011 0000 0000 0000 0000 0000 0000 0000
    LPC_GPIO2->FIODIR |= 0X0000007C;  // LEDs on PORT2 P2.2..6 is output           // 0000 0000 0000 0000 0000 0000 0111 1100
}

void potentiometer_init(void)
{
    LPC_SC->PCONP |= (1 << 12); // enable power to ADC
    
    LPC_PINCON->PINSEL1 &= ~(3<<18); // clear bits
    LPC_PINCON->PINSEL1 |= (1<<18); // set bits P0.25 is AD0.2
    
    LPC_ADC->ADCR =  ( 1 <<  2 ) | // select the second channel
                     ( 4 <<  8 ) | // ADC clock = 25 MHz / 5
                     ( 1 << 21 );  // enable ADC
    
    LPC_ADC->ADINTEN = ( 1 << 8 ); // enable interrupt for all ADC channels
    
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


int main(void)
{
    int joystick_val = 0;
    int int_temperature = 25;
    int ADC_Value = 0;
    unsigned int aDCStat;
    char str_temperature[10];    
    char str_potentiometer[10];
    sprintf(str_temperature, "temp: %d", int_temperature);

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
    
    // start program
    for(;;)
    {
        joystick_val = read_joystick();
        if (joystick_val == 0x08) {
            //GLCD_DisplayString(1,6,1, "   up");
            int_temperature += 1;
            sprintf(str_temperature, "temp: %d", int_temperature);
            GLCD_DisplayString(0, 0, 1, str_temperature);
            
            /* polling */
            // Read the converted value
            LPC_ADC->ADCR |= ( 1 << 24 );
            // start Conversion
            while (LPC_ADC->ADGDR & 0x8000 == 0);
            ADC_Value = (LPC_ADC->ADGDR>>4) & 0xFFF / 10;
            sprintf(str_potentiometer, "pot: %4d", ADC_Value);
            GLCD_DisplayString(1, 0, 1, str_potentiometer);
            
            check_furnace(int_temperature, ADC_Value, led);
            
        } else if (joystick_val == 0x20) {
            //GLCD_DisplayString(1,6,1, " down");
            int_temperature -= 1;
            sprintf(str_temperature, "temp: %d", int_temperature);
            GLCD_DisplayString(0, 0, 1, str_temperature);
            
            /* polling */
            // Read the converted value
            LPC_ADC->ADCR |= ( 1 << 24 );
            // start Conversion
            while (LPC_ADC->ADGDR & 0x8000 == 0);
            ADC_Value = (LPC_ADC->ADGDR>>4) & 0xFFF / 10;
            sprintf(str_potentiometer, "pot: %4d", ADC_Value);
            GLCD_DisplayString(1, 0, 1, str_potentiometer);
            
            check_furnace(int_temperature, ADC_Value, led);
        } 
    }
    return 0;
}


/*
void sampleInterrupts(void)
{
    void NVIC_EnableIRQ(IRQn_Type IRQn);
    void NVIC_DisableIRQ(IRQn_Type IRQn);
  
    void NVIC_SetPriority(IRQn_Type IRQn, int32_t priority);
    uint32_t NVIC_GetPrioritY(IRQn_Type IRQn);
  
    void NVIC_SetPendingIRQ(IRQn_Type IRQn);
    void NVIC_ClearPendingIRQ(IRQn_Type IRQn);
    IRQn_Type NVIC_GetPendingIRQ(IRqn_Type IRQn);
} 

void interrupt_test(void) 
{
    LPC_GPIOINT->IO2IntEnF |= 1 << 10; // falling edge of P2.10
    LPC_GPIOINT->IO2IntEnR |= 1 << 10; // risign edg of P2.10
    NVIC_EnableIRQ(EINT3_IRQn);
}

void timerInit(void)
{
    LPC_TIM0->TCR = 0x02; // reset time
    LPC_TIM0->TCR = 0x01; // enable timer
    LPC_TIM0->MR0 = 2048; // match value (can be anything)
    LPC_TIM0->MCR |= 0x03; // on match, generate interrupt and reset
    NVIC_EnableIRQ(TIMER0_IRQn); // allow interrupts from the timer
}



*/

/* interrupt */
// Read ADC status clears the interrupt condition
//aDCStat = LPC_ADC->ADSTAT;
//ADC_Value = (LPC_ADC->ADGDR >> 4) & 0xFFF;
//sprintf(str_potentiometer, "pot: %d", ADC_Value);
//GLCD_DisplayString(1, 0, 1, str_potentiometer);