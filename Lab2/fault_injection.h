#ifndef FAULT_INJECTION_H
#define FAULT_INJECTION_H

#define RANDOM_FAULT    0
#define STUCK_AT_FAULT  1

#define INTEGER 0
#define DOUBLE 1

#define REDUNDANCY 1
#define VOTING 2
#define HETEROGENEOUS 3
#define VERIFICATION 4

void fault_injection_reset(void);
int faulty_int (int original_value, int fault_type);
double faulty_double (double original_value, int fault_type);

#endif

