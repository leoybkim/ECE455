#include "verification.h"
#include "voting.h"
#include "fault_injection.h"
#include <stdio.h>
#include <math.h>

int inverse(double data, int fault_type) 
{
    double nr_return_value;
	nr_return_value = faulty_double(newton_raphson(data), fault_type);
    if (fabs((nr_return_value * nr_return_value) - data) < EPSILON) 
	{
        printf("%f, %f\n", nr_return_value, data);
        return 0;
    }
	else
	{
        printf("%f, %f\n", nr_return_value, data);
        return 1;
    }
}
