#include <stdio.h>
#include <lpc17xx.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "glcd.h"
#include "inputs.h"
#include "fault_injection.h"
#include "redundancy.h"
#include "voting.h"
#include "heterogeneous.h"
#include "verification.h"


// global variables
int mode; 
int fault_type; 
int data_type;  

double first;
double second;
double third;
double answer;


/* Joystick Manual
   LEFT/RIGHT --> change mode from 1 to 4
   UP         --> RANDOM_FAULT 
   DOWN       --> STUCK_AT_FAULT
   SELECT     --> toggle between INTEGER and DOUBLE */
int joystick_val;     

// custom test sets

double d_test_val;
int i_test_val;

// custom data types
integer_data original_int;
double_data original_double;

// timer to wake up from sleep in the main while loop
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
    
	// default mode/data_type/fault_type
    mode = 1;
	fault_type = RANDOM_FAULT;
	data_type = INTEGER; 
}

double poll_potentiometer() 
{
	// poll potentiometer
	double test_val;
	LPC_ADC->ADCR |= ( 1 << 24 );                     // read the converted value
	while (LPC_ADC->ADGDR & 0x8000 == 0);             // wait for conversion
	test_val = ((LPC_ADC->ADGDR>>4) & 0xFFF);    // conversion to celcius range
	return test_val;
}

// main program
int main()
{
    // debug tags
    char str_mode[12];   
	char str_data_type[12];
	
	char str_joystick[12];
	
	
    char str_faulty_int[12];
    char str_original_int[12];
    //char str_error[12];
    int faulty_int_var;
    
    // initialize
    init_setup();
   
    // start program
    while(1) {
		
		// poll pot
		d_test_val = 1000.00;
		i_test_val = (int) d_test_val;
		
        // poll joystick to read user input changes
        joystick_val = debounce();
        if (joystick_val == 0x08 && mode > 1) 
        {
            mode -= 1;
        } 
        else if (joystick_val == 0x20 && mode < 4) 
        {
            mode += 1;
        }       
		else if (joystick_val == 0x64) 
		{
			GLCD_DisplayString(4, 0, 1, "ARAREWWFWE");
			fault_type = RANDOM_FAULT;
		} 
		else if (joystick_val == 0x16)
		{
			fault_type = STUCK_AT_FAULT;
		}
		else if (joystick_val == 0x01) 
		{
			data_type = !data_type;
		}
						
		
		
		/*switch(mode)
		{
            case REDUNDANCY:
				if (data_type == INTEGER) 
				{
					write_integer(i_test_val, &original_int, fault_type);
					if (read_integer(&original_int, fault_type))
					{
						//printf("error occured \n");
					   fault_injection_reset();
					} 
					else 
					{
						//printf("fine\n");
					}
				}
                else if (data_type == DOUBLE)
				{
					write_double(d_test_val, &original_double, fault_type);
					if (read_double(&original_double, fault_type))
					{
					   //printf("error occured \n");
					   fault_injection_reset();
					} 
					else 
					{
					   //printf("fine\n");
					}
				}
                break;
            case VOTING:
                if (voting_system(d_test_val, fault_type))
				{
                   // printf("ERROR \n");
                }
				else
				{
                    //printf("FINE\n");
                }
                break;
            case HETEROGENEOUS:
                answer = heterogeneous(d_test_val, fault_type);
                if (answer == -1.0) 
				{
                    //printf("ERROR \n");
                } 
				else
				{
                    //printf("FINE %f \n", answer);
                }
                break;
            case VERIFICATION:
                if (inverse(d_test_val, fault_type)) 
				{
                    //printf("ERROR \n");
                } 
				else
				{
                    //printf("FINE \n");
                }
                break;
            default:
                break;
        }*/
        
        // display
		switch(mode)
		{
			case REDUNDANCY:
				strcpy(str_mode, "TEST 1.REDUNDANCY    ");
				break;
			case VOTING:
				strcpy(str_mode, "TEST 2.VOTING        ");
				break;
			case HETEROGENEOUS:
				strcpy(str_mode, "TEST 3.HETEROGENEOUS ");
				break;
			case VERIFICATION:
				strcpy(str_mode, "TEST 4.VERIFICATION  ");
				break;
			default:
				break;
		}
        GLCD_DisplayString(0, 0, 1, (unsigned char*) str_mode);
		
		if (data_type == INTEGER)
		{
			strcpy(str_data_type, "DATA TYPE: INTEGER");
		}
		else 
		{
			strcpy(str_data_type, "DATA TYPE: DOUBLE ");
		}
		GLCD_DisplayString(1, 0, 1, (unsigned char*) str_data_type);
		
        sprintf(str_original_int, "Org: %f", d_test_val);
        GLCD_DisplayString(5, 0, 1, (unsigned char*) str_original_int);
        sprintf(str_faulty_int, "Calc: %d", faulty_int_var);
        GLCD_DisplayString(6, 0, 1, (unsigned char*)str_faulty_int);
        
        sprintf(str_joystick, "%d", joystick_val);
		GLCD_DisplayString(7, 0, 1, (unsigned char*) str_joystick);
		
        __WFI();  // stay in low power mode until interrupt occurs
    }
    
    //return 1;	
}
