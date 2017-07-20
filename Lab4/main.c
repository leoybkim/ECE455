#include <stdio.h>
#include <lpc17xx.h>
#include <math.h>
#include <string.h>
#include "glcd.h"
#include "inputs.h"

// initial board setup
void init_setup()
{
    SystemInit();
    GLCD_Init();
    GLCD_Clear(White);
}

// TODO: 2 push button to engage cruise control and brake
// TODO: joystick to changet the target speed while in engaged mode
// TODO: value measured from potentiometer will be the speed of vehicle

// intuition: reduce the throttle, speed will decrease --> simulated by potentiometer

// TODO: hazard anaylsis to determinthe ASIL level
// The report needs to include the ASIL level that
// you designate to the component, along with the
// Exposure, Severity and Controlability values for
// each scenario you think up

int main() 
{
	init_setup();
	GLCD_DisplayString(0,0,1,"Hello World!");
	return 0;
}
