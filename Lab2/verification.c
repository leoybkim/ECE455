#include "verification.h"
#include "voting.h"
#include "fault_injection.h"
#include <stdio.h>
#include <math.h>
#include "glcd.h"


char str_error4[20];
int inverse(double data, int fault_type) 
{
    double nr_return_value;
	nr_return_value = faulty_double(newton_raphson(data), fault_type);
    if (fabs((nr_return_value * nr_return_value) - data) < EPSILON) 
	{
        return 0;
    }
	else
	{
		sprintf(str_error4, "f:%.3f", nr_return_value);
		GLCD_DisplayString(7, 0, 1, (unsigned char*) str_error4);
        return 1;
    }
}
