#include "voting.h"
#include "fault_injection.h"
#include <math.h>
#include <stdio.h>
#include "glcd.h"

char str_error21[20];
char str_error22[20];
char str_error23[20];
double newton_raphson(double a) 
{
    double value = 1;
	int i;
    for (i = 0; i < ITERATION; i++)
	{
        value = (0.5)*(value + a/value);
    }
    return value;
}

double newton_raphson_amplified(double a, int fault_type) 
{
    double value = 1;
	int i;
    for (i = 0; i < ITERATION; i++)
	{
        value = (0.5)*(value + a/value);
		value = faulty_double(value, fault_type);
    }
    return value;
}

// calls fault injection reset after every trials
int voting_system1(double a, int fault_type)
{
    double trial_1;
    double trial_2;
    double trial_3;

    trial_1 = newton_raphson(faulty_double(a, fault_type));
    fault_injection_reset();
    trial_2 = newton_raphson(faulty_double(a, fault_type));
    fault_injection_reset();
    trial_3 = newton_raphson(faulty_double(a, fault_type));
    fault_injection_reset();

    if (!( (fabs(trial_1 - trial_2) < EPSILON) || (fabs(trial_2 - trial_3) < EPSILON)
     || (fabs(trial_1 - trial_3) < EPSILON) ))
	{
		sprintf(str_error21, "t1:%.7f", trial_1);
		sprintf(str_error22, "t2:%.7f", trial_2);
		sprintf(str_error23, "t3:%.7f", trial_3);
		GLCD_DisplayString(7, 0, 1, (unsigned char*) str_error21);
		GLCD_DisplayString(8, 0, 1, (unsigned char*) str_error22);
		GLCD_DisplayString(9, 0, 1, (unsigned char*) str_error23);
        return 1;
    }
	
    return 0;
}

// calls fault injection reset after 3 trials
int voting_system2(double a, int fault_type)
{
    double trial_1;
    double trial_2;
    double trial_3;

    trial_1 = newton_raphson(faulty_double(a, fault_type));
    trial_2 = newton_raphson(faulty_double(a, fault_type));
    trial_3 = newton_raphson(faulty_double(a, fault_type));
    fault_injection_reset();

    if (!( (fabs(trial_1 - trial_2) < EPSILON) || (fabs(trial_2 - trial_3) < EPSILON)
     || (fabs(trial_1 - trial_3) < EPSILON) ))
	{
		sprintf(str_error21, "t1:%.7f", trial_1);
		sprintf(str_error22, "t2:%.7f", trial_2);
		sprintf(str_error23, "t3:%.7f", trial_3);
		GLCD_DisplayString(7, 0, 1, (unsigned char*) str_error21);
		GLCD_DisplayString(8, 0, 1, (unsigned char*) str_error22);
		GLCD_DisplayString(9, 0, 1, (unsigned char*) str_error23);
        return 1;
    }
	
    return 0;
}
