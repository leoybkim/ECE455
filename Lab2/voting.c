#include "voting.h"
#include "fault_injection.h"
#include <math.h>
#include <stdio.h>

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

int voting_system(double a, int fault_type)
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
        printf("%f, %f, %f\n", trial_1, trial_2, trial_3);
        return 1;
    }
	else 
	{
        printf("%f, %f, %f\n", trial_1, trial_2, trial_3);
    }

    return 0;
}
