#include <stdio.h>
#include <lpc17xx.h>
#include "glcd.h"
#include "inputs.h"
#include "fault_injection.h"

// global variables
int mode;
int joystick_val;     // determine direction (up or down)

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
    
    mode = 0;
    joystick_val = 0;
}




int main(void)
{
    // debug tags
    char str_mode[10];       
    
    
    init_setup();
   
    for(;;) {
        
        // poll joystick
        joystick_val = debounce();
        if (joystick_val == 0x08 && mode > 0) {
            mode -= 1;
        } else if (joystick_val == 0x20 && mode < 4) {
            mode += 1;
        }
        
        sprintf(str_mode, "Test mode: %d", mode);
        GLCD_DisplayString(0, 0, 1, str_mode);
        
        
        __WFI();  // stay in low power mode until interrupt occurs
    }
    
    //return 0;
}
