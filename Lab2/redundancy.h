#ifndef REDUNDANCY_H
#define REDUNDANCY_H

typedef struct INTEGER_DATA {
    int i_data;
    int i_negated_data;
} integer_data;

typedef struct DOUBLE_DATA {
    double d_data;
    double d_negated_data;
} double_data;

int read_integer(integer_data *data);
void write_integer(int value, integer_data *data);
int read_double(double_data *data);
void write_double(double value, double_data *data);

#endif
