#ifndef VOTING_H
#define VOTING_H

#define EPSILON 1e-6
#define ITERATION 100

double newton_raphson(double a);
double newton_raphson_amplified(double a, int fault_type);
int voting_system1(double a, int fault_type);
int voting_system2(double a, int fault_type);


#endif
