#include <stdint.h>
#include <stdio.h>
#include "redundancy.h"
#include "fault_injection.h"


// you will set up a mechanism to store data in duplicate form, with one of the
// “clones” stored in negated form (i.e., the bits inverted). The negated duplicate facilitates early detection
// of stuck-at faults — a bit in the hardware may be stuck at some particular value, and this fault
// could go undetected (and potentially lead to an error and failure at a later time) if the value being
// accessed happens to have the same value at the faulty bit.
// A suggested way to implement this functionality in C is setting up a user-defined type (a struct) for
// each data type (for the purpose of this lab, we will only deal with int and double) that contains two
// data members: the value and the negated value. You will also provide a function to read the value
// and a function to set the value. The read function will return a success code indicating whether a
// fault was detected and will write the retrieved value in a parameter passed by reference.



// TODO: make argument for STUCK_AT_FAULT or RANDOM_FAULT

// returns 0 if original and test value matches up
// returns 1 if fault is detected
int read_integer(integer_data *data) {
    int test_value = ~faulty_int(data->i_data, STUCK_AT_FAULT);
    if (data->i_negated_data == test_value) {
        return 0;
    } else {
        printf("data: %d, test: %d\n", data->i_negated_data, test_value);
        return 1;
    }
}

void write_integer(int value, integer_data *data) {
    data -> i_data = value;
    data -> i_negated_data = ~value;
    //printf("data: %d, neg_data: %d\n", data->i_data, data->i_negated_data);
}

int read_double(double_data *data) {
    double temp = faulty_double(data->d_data, RANDOM_FAULT);
    //double temp = data->d_data;
    uint64_t * ptr = (uint64_t *) &temp;
    uint64_t pre_negated = ~(*ptr);
    double test_value = (double) pre_negated;
    if (data->d_negated_data == test_value) {
        return 0;
    } else {
        printf("data: %f, test: %f\n", data->d_negated_data, test_value);
        return 1;
    }
}

void write_double(double value, double_data *data) {
		uint64_t * ptr;
		uint64_t pre_negated;
	
    data -> d_data = value;
		ptr = (uint64_t *) &value;
    pre_negated = ~(*ptr);
    data -> d_negated_data = (double) pre_negated;
    //printf("data: %f, neg: %f\n", data->d_data, data->d_negated_data);
}
