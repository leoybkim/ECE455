#include <stdio.h>
#include <lpc17xx.h>
#include "glcd.h"
#include "inputs.h"
#include "fault_injection.h"

// global variables
int mode;
int joystick_val;     // determine direction (up or down)
int test_val[] = {1, 100, 10000, 99999999, 123456789, 6477019364};

// timer to wake up from sleep
void timer0_init(void)
{
    LPC_TIM0->TCR = 0x02;         // reset time
    LPC_TIM0->TCR = 0x01;         // enable timer
    LPC_TIM0->MR0 = 4000000;      // match value (can be anything)
    LPC_TIM0->MCR |= 0x03;        // on match, generate interrupt and reset
    NVIC_EnableIRQ(TIMER0_IRQn);  // allow interrupts from the timer
}

void TIMER0_IRQHandler(void)
{
    if((LPC_TIM0->IR & 0x01) == 0x01) // if MR0 interrupt
    {
        LPC_TIM0->IR |= 1 << 0; 
    }
}


// initial board setup
void init_setup() 
{
    SystemInit();
    GLCD_Init();    
    GLCD_Clear(White);  
    joystick_init();
    potentiometer_init();
    led_init();
    timer0_init();
    
    mode = 1;
}




int main(void)
{
    // debug tags
    char str_mode[10];   
    char str_faulty_int[10];
    char str_original_int[10];
    char str_error[10];
    int faulty_int_var;
    
    // initialize
    init_setup();
   
    // start program
    for(;;) {
        
        // poll joystick
        joystick_val = debounce();
        if (joystick_val == 0x08 && mode > 1) 
        {
            mode -= 1;
        } 
        else if (joystick_val == 0x20 && mode < 4) 
        {
            mode += 1;
        }
        
        // faulty test??
        faulty_int_var = faulty_int(test_val[5], RANDOM_FAULT);
        if (faulty_int_var != test_val[5]) 
        {
            // TODO: wait for an interrupt to continue
            sprintf(str_error, "ERROR OCCCURED: %d", faulty_int_var);
            GLCD_DisplayString(5, 0, 1, str_error);
        }
        
        // display
        sprintf(str_mode, "Test mode: %d", mode);
        GLCD_DisplayString(0, 0, 1, str_mode);
        sprintf(str_original_int, "Original int: %d", test_val[5]);
        GLCD_DisplayString(1, 0, 1, str_original_int);
        sprintf(str_faulty_int, "Faulty int: %d", faulty_int_var);
        GLCD_DisplayString(2, 0, 1, str_faulty_int);
        
        
        __WFI();  // stay in low power mode until interrupt occurs
    }
    
    //return 0;
}
