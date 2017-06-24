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
int item;
int fault_type; 
int data_type;  
int flag;

/* Joystick Manual
   LEFT/RIGHT --> change mode from 1 to 4
   UP         --> RANDOM_FAULT 
   DOWN       --> STUCK_AT_FAULT
   SELECT     --> toggle between INTEGER and DOUBLE */
int joystick_val;     

// custom test sets


double d_test_val_arr[] = {1.23, 10.00, 1234.56, 999999.99, 202020.20};
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
    
	// SET MODES HERE
	mode = 1;
	//fault_type = RANDOM_FAULT;
	fault_type = STUCK_AT_FAULT;
	//data_type = INTEGER; 
	data_type = DOUBLE; 
	flag = 0;
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

// voting system cant check STUCK AT 

// main program
int main()
{
    // debug tags
    char str_mode[20];   
	char str_data_type[20];
	char str_fault_type[20];
    char str_original_int[20];

    // initialize
    init_setup();
   
    // start program
    while(1) {
		
		// SET TEST VALUE HERE
		//d_test_val = 1.23;
		//d_test_val = 10.00;
		//d_test_val = 1234.56;
		//d_test_val = 9999999.99;
		//d_test_val = 202020.20;

		
		i_test_val = (int) d_test_val;
		
		// poll joystick to read user input changes
		/*joystick_val = debounce();
		if (joystick_val == 0x08 && mode > 1) 
		{
			mode -= 1;
		} 
		else if (joystick_val == 0x20 && mode < 4) 
		{
			mode += 1;
		}       
		else if (joystick_val == 0x10) 
		{
			fault_type = RANDOM_FAULT;
		} 
		else if (joystick_val == 0x40)
		{
			fault_type = STUCK_AT_FAULT;
		}
		else if (joystick_val == 0x01) 
		{
			data_type = !data_type;
		}*/
		
		
		joystick_val = debounce();
		if (joystick_val == 0x08 && item > 0) 
		{
			item -= 1;
		} 
		else if (joystick_val == 0x20 && item < 5) 
		{
			item += 1;
		}       
		
		d_test_val = d_test_val_arr[item];
			
		switch(mode)
		{
            case REDUNDANCY:
				if (data_type == INTEGER) 
				{
					write_integer(i_test_val, &original_int, fault_type);
					if (read_integer(&original_int, fault_type))
					{
						flag = 1;
					} 
				}
                else if (data_type == DOUBLE)
				{
					write_double(d_test_val, &original_double, fault_type);
					if (read_double(&original_double, fault_type))
					{
						flag = 1;
					} 
				}
                break;
            case VOTING:
                /*if (voting_system1(d_test_val, fault_type))
				{
					flag = 1;
                }*/
				if (fault_type == RANDOM_FAULT)
				{
					if (voting_system1(d_test_val, fault_type))
					{
						flag = 1;
					}
				} 
				else if (fault_type == STUCK_AT_FAULT)
				{
					if (voting_system2(d_test_val, fault_type))
					{
						flag = 1;
					}
				}
				
                break;
            case HETEROGENEOUS:
                /*if (heterogeneous1(d_test_val, fault_type) == -1.0) 
				{
					flag = 1;
                }*/ 
				if (fault_type == RANDOM_FAULT)
				{
					if ((heterogeneous1(d_test_val, fault_type) == -1.0))
					{
						flag = 1;
					}
				} 
				else if (fault_type == STUCK_AT_FAULT)
				{
					if ((heterogeneous2(d_test_val, fault_type) == -1.0))
					{
						flag = 1;
					}
				}
                break;
            case VERIFICATION:
                /*if (inverse(d_test_val, fault_type)) 
				{
					flag = 1;
                } */
				if (inverse2(d_test_val, fault_type)) 
				{
					flag = 1;
                }
                break;
            default:
                break;
        }
        
        // display
		switch(mode)
		{
			case REDUNDANCY:
				strcpy(str_mode, "REDUNDANCY   ");
				break;
			case VOTING:
				strcpy(str_mode, "VOTING       ");
				break;
			case HETEROGENEOUS:
				strcpy(str_mode, "HETEROGENEOUS");
				break;
			case VERIFICATION:
				strcpy(str_mode, "VERIFICATION ");
				break;
			default:
				break;
		}
		

        GLCD_DisplayString(0, 0, 1, (unsigned char*) str_mode);
		
		if (data_type == INTEGER)
		{
			strcpy(str_data_type, "INTEGER");
			sprintf(str_original_int, "i:%d        ", i_test_val);
		}
		else if (data_type == DOUBLE)
		{
			strcpy(str_data_type, "DOUBLE ");
			sprintf(str_original_int, "i:%.3f      ", d_test_val);
		}
		
		GLCD_DisplayString(1, 0, 1, (unsigned char*) str_data_type);
		
		if (fault_type == RANDOM_FAULT)
		{
			strcpy(str_fault_type, "RANDOM");
		}
		else if (fault_type == STUCK_AT_FAULT)
		{
			strcpy(str_fault_type, "STUCK ");
		}
		GLCD_DisplayString(2, 0, 1, (unsigned char*) str_fault_type);
        GLCD_DisplayString(4, 0, 1, (unsigned char*) str_original_int);
		
		// if error found
		if (flag) 
		{
			break;
		}
		

        __WFI();  // stay in low power mode until interrupt occurs
    }
	
    //GLCD_DisplayString(3, 0, 1, "FAULTY FOUND");
    return 0;		
}
