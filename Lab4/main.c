#include <stdio.h>
#include <stdlib.h>
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
double target_speed;
double measured_speed;

/* engine throttle in % */
//TODO change to floats
double throttle;

// initial board setup
void init_setup (void)
{
    SystemInit();
    GLCD_Init();
    GLCD_Clear(White);
	GLCD_DisplayString(0,0,1,"HOLD BUTTON 100ms");

    init_joystick();
	init_potentiometer();

	engaged = FALSE;
	brake = FALSE;
    target_speed = 0;
    measured_speed = 0;
    throttle = 0;
}

// setup timer
void init_timer0 (void)
{
    LPC_TIM0->TCR = 0x02;         // reset time
    LPC_TIM0->TCR = 0x01;         // enable timer
    LPC_TIM0->MR0 = 25e6;         // 100 ms, 100 Mhz /4 = 25 Mhz
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

int main (void)
{
	 /* Debugging tools */
    //char str_joystick[20];
	char str_potentiometer[20];
	char str_target_speed[20];
	char str_throttle[20];

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
		sprintf(str_potentiometer, "MEASURED: %d   ", potentiometer_val);
		GLCD_DisplayString(2, 0, 1, (unsigned char*) str_potentiometer);

		/* Poll Joystick */
        joystick_val = read_joystick();
        if (joystick_val == K_UP)
		{
			brake = FALSE;
        	GLCD_DisplayString(1, 0, 1, "JOYSTICK: UP     ");
            if (engaged)
            {
                target_speed += 5.0;
            }
        }
        else if (joystick_val == K_DOWN)
		{
			brake = FALSE;
        	GLCD_DisplayString(1, 0, 1, "JOYSTICK: DOWN   ");
            if (engaged)
            {
                target_speed -= 5.0;
            }
        }
        else if (joystick_val == K_LEFT)
		{
			brake = FALSE;
			engaged = FALSE;
        	GLCD_DisplayString(1, 0, 1, "JOYSTICK: LEFT   ");
        }
        else if (joystick_val == K_RIGHT)
		{
			brake = FALSE;
			engaged = TRUE;
        	GLCD_DisplayString(1, 0, 1, "JOYSTICK: RIGHT  ");
        }
        else if (joystick_val == K_SELECT)
		{
			brake = TRUE;
			engaged = FALSE;
			GLCD_DisplayString(1, 0, 1, "JOYSTICK: SELECT ");
		}
		else {
			brake = FALSE;
			GLCD_DisplayString(1, 0, 1, "JOYSTICK:        ");
		}

		/* Cruise control display */
		if (engaged == TRUE)
		{
            /* when transitioning from disengaged to engaged */
            if (target_speed == 0)
            {
    			target_speed = (double) potentiometer_val; // set initial target speed as current measured speed
                throttle = 50.0;
            }

            /* feedback loop */
            measured_speed = (double) potentiometer_val;
            if (abs(target_speed - measured_speed) > 2.5)
            {
                // change throttle
                throttle += K * (target_speed - measured_speed);
                if (throttle > 100.0) {
                    throttle = 100.0;
                }
                else if (throttle < 0)
                {
                    throttle = 0;
                }
            }

			sprintf(str_target_speed, "TARGET: %.2f   ", target_speed);
			sprintf(str_throttle, "THROTTLE: %.2f    ", throttle);
			GLCD_DisplayString(5, 0, 1, "CRUISE CTRL: ON  ");
			GLCD_DisplayString(6, 0, 1, (unsigned char*) str_target_speed);
			GLCD_DisplayString(7, 0, 1, (unsigned char*) str_throttle);
		}
		else
		{
            target_speed = 0;
			throttle = 0;
			GLCD_DisplayString(5, 0, 1, "CRUISE CTRL: OFF  ");
			GLCD_DisplayString(6, 0, 1, "TARGET:      ");
			GLCD_DisplayString(7, 0, 1, "THROTTLE:      ");
		}

		/* Brake display */
		if (brake == TRUE)
		{
			GLCD_DisplayString(8, 0, 1, "BRAKE: ON  ");
		}
		else
		{
			GLCD_DisplayString(8, 0, 1, "BRAKE: OFF  ");
		}


        __WFI(); // stay in low power mode until interrupt occurs
	}

	//return 0;
}
