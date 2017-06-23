#include <stdint.h>
#include <stdio.h>
#include "redundancy.h"
#include "fault_injection.h"


// returns 0 if original and test value matches up
// returns 1 if fault is detected
int read_integer(integer_data *data, int fault_type) 
{
    int test_value = ~faulty_int(data->i_data, fault_type);
    if (data->i_negated_data == test_value) 
	{
        return 0;
    } else 
	{
        printf("data: %d, test: %d\n", data->i_negated_data, test_value);
        return 1;
    }
}

void write_integer(int value, integer_data *data, int fault_type) 
{
    data -> i_data = value;
    data -> i_negated_data = ~value;
    //printf("data: %d, neg_data: %d\n", data->i_data, data->i_negated_data);
}

int read_double(double_data *data, int fault_type)
{
    double temp = faulty_double(data->d_data, fault_type);
    //double temp = data->d_data;
    uint64_t * ptr = (uint64_t *) &temp;
    uint64_t pre_negated = ~(*ptr);
    double test_value = (double) pre_negated;
    if (data->d_negated_data == test_value) 
	{
        return 0;
    }
	else
	{
        printf("data: %f, test: %f\n", data->d_negated_data, test_value);
        return 1;
    }
}

void write_double(double value, double_data *data, int fault_type) 
{
	uint64_t * ptr;
	uint64_t pre_negated;

    data -> d_data = value;
	ptr = (uint64_t *) &value;
    pre_negated = ~(*ptr);
    data -> d_negated_data = (double) pre_negated;
    //printf("data: %f, neg: %f\n", data->d_data, data->d_negated_data);
}
