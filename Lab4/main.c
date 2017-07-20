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


int main() 
{
	init_setup();
	GLCD_DisplayString(0,0,1,"Hello World!");
	return 0;
}
