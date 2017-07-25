#include <stdio.h>
#include <lpc17xx.h>
#include <math.h>
#include <string.h>
#include "glcd.h"
#include "joystick.h"
#include "potentiometer.h"

typedef int bool;
#define TRUE 1
#define FALSE 0

/* constants */
#define K 1 // proportionality factor

/* state */
bool engaged;
bool brake;

/* car speed km/h */
//TODO change to floats
uint32_t target_speed;
uint32_t measured_speed;

/* engine throttle in % */
//TODO change to floats
uint32_t throttle;

// initial board setup
void init_setup (void)
{
    SystemInit();
    GLCD_Init();
    GLCD_Clear(White);
	// GLCD_DisplayString(0,0,1,"Hello World!");

    init_joystick();
	init_potentiometer();

	engaged = FALSE;
	brake = FALSE;
    target_speed = 0;
    measured_speed = 0;
    throttle = 0;
}

// setup timer
// TODO 100 ms
void init_timer0 (void)
{
    LPC_TIM0->TCR = 0x02;         // reset time
    LPC_TIM0->TCR = 0x01;         // enable timer
    LPC_TIM0->MR0 = 4000000;      // match value (can be anything)
    LPC_TIM0->MCR |= 0x03;        // on match, generate interrupt and reset
    NVIC_EnableIRQ(TIMER0_IRQn);  // allow interrupts from the timer
}

// timer interrupt handler
void TIMER0_IRQHandler (void)
{
    if((LPC_TIM0->IR & 0x01) == 0x01) // if MR0 interrupt
    {
        LPC_TIM0->IR |= 1 << 0;
    }
}

// TODO: 2 push button to engage cruise control and brake
/*When the driver pushes the \engage" button, the current vehicle speed becomes the target speed, and
the cruise control system will attempt to maintain that vehicle speed. To this end, the cruise control
system implements a feedback loop where it actuates the throttle; the measured (actual) vehicle
speed is the feedback parameter. Notice that the brakes are never actuated by the cruise control
system|given the air drag/resistance force, it is assumed that if we reduce the applied throttle, the
speed will lower.*/

// TODO: joystick to changet the target speed while in engaged mode
/*While engaged, the driver can use two buttons, UP and DOWN, to adjust the target speed. Both
buttons change the target speed by 5 km/h.*/


// TODO: value measured from potentiometer will be the speed of vehicle
/*You will use the board's potentiometer to represent the measured vehicle speed (you should convert
the A/D measurement to an actual speed, for example between 0 and 100 km/h), and the LCD will
output the throttle that the system is applying, the actual vehicle speed, and the measured speed.*/



// intuition: reduce the throttle, speed will decrease --> simulated by potentiometer

// TODO: hazard anaylsis to determinthe ASIL level
// The report needs to include the ASIL level that
// you designate to the component, along with the
// Exposure, Severity and Controlability values for
// each scenario you think up and all the requirements for the ASIL level chosen
// and implement those requirements
// ISO-26262 analysis

/*1) The ASIL level of the component with clear
arguments as to why you arrived to that conclusion
2) List of requirements that should be implemented
for the associated ASIL level*/


int main (void)
{
	 /* Debugging tools */
    char str_joystick[20];
	char str_potentiometer[20];
	char str_target_speed[20];

    /* variables */
    uint32_t joystick_val = 0;
	uint32_t potentiometer_val = 0;

	init_setup();
	init_timer0();

	while(TRUE)
	{
		/* Poll Potentiometer */
		potentiometer_conversion();
		potentiometer_val = read_potentiometer();
		potentiometer_val = potentiometer_val / 40; // range to car speed km/h
		sprintf(str_potentiometer, "POTMETER: %d   ", potentiometer_val);
		GLCD_DisplayString(1, 0, 1, (unsigned char*) str_potentiometer);

		/* Poll Joystick */
        joystick_val = read_joystick();
        if (joystick_val == K_UP)
		{
			brake = FALSE;
        	GLCD_DisplayString(0, 0, 1, "JOYSTICK: UP     ");
            if (engaged)
            {
                target_speed += 5;
            }
        }
        else if (joystick_val == K_DOWN)
		{
			brake = FALSE;
        	GLCD_DisplayString(0, 0, 1, "JOYSTICK: DOWN   ");
            if (engaged)
            {
                target_speed -= 5;
            }
        }
        else if (joystick_val == K_LEFT)
		{
			brake = FALSE;
			engaged = FALSE;
        	GLCD_DisplayString(0, 0, 1, "JOYSTICK: LEFT   ");
        }
        else if (joystick_val == K_RIGHT)
		{
			brake = FALSE;
			engaged = TRUE;
        	GLCD_DisplayString(0, 0, 1, "JOYSTICK: RIGHT  ");
        }
        else if (joystick_val == K_SELECT)
		{
			brake = TRUE;
			engaged = FALSE;
			GLCD_DisplayString(0, 0, 1, "JOYSTICK: SELECT ");
		}
		else {
			brake = FALSE;
			GLCD_DisplayString(0, 0, 1, "JOYSTICK:        ");
		}

		/* Cruise control display */
		if (engaged == TRUE)
		{
            /* when transitioning from disengaged to engaged */
            if (target_speed == 0)
            {
    			target_speed = potentiometer_val; // set initial target speed as current measured speed
                throttle = 50;
            }

            /* feedback loop */
            measured_speed = potentiometer_val;
            if (abs(target_speed - measured_speed) > 2.5)
            {
                // change throttle
                throttle += K * (target_speed - measured_speed);
                if (throttle > 100) {
                    throttle = 100;
                }
                else if (throttle < 0)
                {
                    throttle = 0;
                }
            }

			sprintf(str_target_speed, "TARGET_V: %d   ", target_speed);
			GLCD_DisplayString(4, 0, 1, "CRUISE CTRL: ON  ");
			GLCD_DisplayString(5, 0, 1, (unsigned char*) str_target_speed);
		}
		else
		{
            target_speed = 0;
			GLCD_DisplayString(4, 0, 1, "CRUISE CTRL: OFF  ");
			GLCD_DisplayString(5, 0, 1, "TARGET_V:      ");
		}

		/* Brake display */
		if (brake == TRUE)
		{
			GLCD_DisplayString(6, 0, 1, "BRAKE: ON  ");
		}
		else
		{
			GLCD_DisplayString(6, 0, 1, "BRAKE: OFF  ");
		}


        __WFI(); // stay in low power mode until interrupt occurs
	}

	return 0;
}
