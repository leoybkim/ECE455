#include <stdio.h>
#include <lpc17xx.h>
#include <stdlib.h>
#include "glcd.h"
#include "inputs.h"
#include "fault_injection.h"
#include "redundancy.h"
#include "voting.h"
#include "heterogeneous.h"
#include "verification.h"


// global variables
int mode;
int type;
double first;
double second;
double third;
double answer;

// input
// LEFT and RIGHT --> change mode from 1 to 4
// UP and DOWN --> RANDOM_FAULT vs STUCK_AT_FAULT
// PRESS CENTER --> toggle between INT vs DOUBLE
int joystick_val;     

// test numbers
int test_val[] = {1, 100, 10000, 99999999, 123456789, 6477019364};

// custom data types
integer_data original_int;
double_data original_double;

// timer to wake up from sleep in main while loop
void timer0_init(void)
{
    LPC_TIM0->TCR = 0x02;         // reset time
    LPC_TIM0->TCR = 0x01;         // enable timer
    LPC_TIM0->MR0 = 4000000;      // match value (can be anything)
    LPC_TIM0->MCR |= 0x03;        // on match, generate interrupt and reset
    NVIC_EnableIRQ(TIMER0_IRQn);  // allow interrupts from the timer
}

// interrupt handler
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
		type = RANDOM_FAULT;
}

int main()
{
    // debug tags
    char str_mode[12];   
    char str_faulty_int[12];
    char str_original_int[12];
    //char str_error[12];
    int faulty_int_var;
    
    // initialize
    init_setup();
   
    // start program
    while(1) {
        
        // poll joystick to read mode changes
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
        /*faulty_int_var = faulty_int(test_val[5], RANDOM_FAULT);
        if (faulty_int_var != test_val[5]) 
        {
            // TODO: wait for an interrupt to continue
            sprintf(str_error, "ERR: %d", faulty_int_var);
            GLCD_DisplayString(5, 0, 1, str_error);
        }*/
				
		/*switch(mode){
            case 1:
                write_integer(test_val[3], &original_int);
                if (read_integer(&original_int)){
					//printf("error occured \n");
                   fault_injection_reset();
                } else {
                    //printf("fine\n");
                }

                // TODO: fix!!
                write_double(100.00, &original_double);
                if (read_double(&original_double)){
                   //printf("error occured \n");
                   fault_injection_reset();
                } else {
                   //printf("fine\n");
                }

                break;
            case 2:
                if (voting_system(100.00)) {
                   // printf("ERROR \n");
                } else {
                    //printf("FINE\n");
                }
                break;
            case 3:
                answer = heterogeneous(100.00);
                if (answer == -1.0) {
                    //printf("ERROR \n");
                } else {
                    //printf("FINE %f \n", answer);
                }
                break;
            case 4:
                if (inverse(10000.00)) {
                    //printf("ERROR \n");
                } else {
                    //printf("FINE \n");
                }
                break;
            default:
                break;
        }*/
        
        // display
        sprintf(str_mode, "Mode: %d", mode);
        GLCD_DisplayString(0, 0, 1, str_mode);
        sprintf(str_original_int, "Org: %d", test_val[5]);
        GLCD_DisplayString(1, 0, 1, str_original_int);
        sprintf(str_faulty_int, "Calc: %d", faulty_int_var);
        GLCD_DisplayString(2, 0, 1, str_faulty_int);
        
        
        __WFI();  // stay in low power mode until interrupt occurs
    }
    
    return 1;	// not going to reach here but just in case
}
