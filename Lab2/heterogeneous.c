#include <stdio.h>
#include <math.h>
#include "heterogeneous.h"
#include "voting.h"
#include "fault_injection.h"
#include "glcd.h"

char str_error31[20];
char str_error32[20];
char str_error33[20];
// standard C sqrt library
double c_sqrt(double data)
{
    return sqrt(data);
}

// approximation using binary search
// start = 1, mid = data/2, end = data
// if mid*mid > end, choose bottom half
// else choose top half
// repeat until start > end
double binary_search(double data) 
{
    double root = 0.0;
    double mid;
    double start = 0.0;
    double end = data;

    while(start <= end) 
	{
        mid = (start + end)/2;
        root = mid;
        if (fabs((mid * mid) - data) <= EPSILON)
		{
            break;
        } 
		else if ((mid * mid) < data) 
		{
            start = mid;
        } 
		else
		{
            end = mid;
        }
    }
    return root;
}



double heterogeneous1 (double a, int fault_type) 
{
    double result_1;
    double result_2;
    double result_3;

    result_1 = faulty_double(c_sqrt(a), fault_type);
    fault_injection_reset();
    result_2 = faulty_double(newton_raphson(a), fault_type);
    fault_injection_reset();
    result_3 = faulty_double(binary_search(a), fault_type);
    fault_injection_reset();

    if ( fabs(result_1 - result_2) < EPSILON || fabs(result_1 - result_3) < EPSILON) 
	{
        return result_1;
    }
	else if (fabs(result_2 - result_3) < EPSILON) 
	{
        return result_2;
    } 
	else
	{
		sprintf(str_error31, "r1:%.7f", result_1);
		sprintf(str_error32, "r2:%.7f", result_2);
		sprintf(str_error33, "r3:%.7f", result_3);
		GLCD_DisplayString(7, 0, 1, (unsigned char*) str_error31);
		GLCD_DisplayString(8, 0, 1, (unsigned char*) str_error32);
		GLCD_DisplayString(9, 0, 1, (unsigned char*) str_error33);

        return -1.0;
    }
}

double heterogeneous2 (double a, int fault_type) 
{
    double result_1;
    double result_2;
    double result_3;

    result_1 = faulty_double(c_sqrt(a), fault_type);
    result_2 = faulty_double(newton_raphson(a), fault_type);
    result_3 = faulty_double(binary_search(a), fault_type);
    fault_injection_reset();

    if ( fabs(result_1 - result_2) < EPSILON || fabs(result_1 - result_3) < EPSILON) 
	{
        return result_1;
    }
	else if (fabs(result_2 - result_3) < EPSILON) 
	{
        return result_2;
    } 
	else
	{
		sprintf(str_error31, "r1:%.7f", result_1);
		sprintf(str_error32, "r2:%.7f", result_2);
		sprintf(str_error33, "r3:%.7f", result_3);
		GLCD_DisplayString(6, 0, 1, (unsigned char*) str_error31);
		GLCD_DisplayString(7, 0, 1, (unsigned char*) str_error32);
		GLCD_DisplayString(8, 0, 1, (unsigned char*) str_error33);

        return -1.0;
    }
}
