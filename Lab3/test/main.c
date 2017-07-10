#include <stdio.h>
#include <math.h>
#include "sensors.h"

typedef int bool;
#define TRUE 1
#define FALSE 0
// #define min(x,y) ((x) <= (y)) ? (x) : (y)

double ACCELERATION_L1[] = {0.0, 5.0, 5.0, 3.0, 2.0, 1.0, 1.0, 1.0, 8.0, 8.0, 5.0, 2.0};
double ACCELERATION_L2[] = {0.0, 5.0, 5.0, 3.0, 2.0, 1.0, 1.0, 1.0, 8.0, 8.0, 5.0, 2.0};
double ACCELERATION_R1[] = {0.0, 5.0, 5.0, 3.0, 2.0, 1.0, 1.0, 1.0, 8.0, 8.0, 5.0, 2.0};
double ACCELERATION_R2[] = {0.0, 5.0, 5.0, 3.0, 2.0, 1.0, 1.0, 1.0, 8.0, 8.0, 5.0, 2.0};
double SPEED_L1[] =  {30.0, 25.0, 20.0, 17.0, 15.0, 13.0, 12.0, 11.0, 10.0, 2.0, 10.0, 15.0, 20.0, 22.0};
double SPEED_L2[] =  {30.0, 25.0, 20.0, 17.0, 15.0, 13.0, 12.0, 11.0, 10.0, 2.0, 10.0, 15.0, 20.0, 22.0};
double SPEED_R1[] = {30.0, 25.0, 20.0, 17.0, 15.0, 13.0, 12.0, 11.0, 10.0, 2.0, 10.0, 15.0, 20.0, 22.0};
double SPEED_R2[] = {30.0, 25.0, 20.0, 17.0, 15.0, 13.0, 12.0, 11.0, 10.0, 2.0, 10.0, 15.0, 20.0, 22.0};

bool F_ACCELERATION_L1 = FALSE;
bool F_ACCELERATION_L2 = FALSE;
bool F_ACCELERATION_R1 = FALSE;
bool F_ACCELERATION_R2 = FALSE;
bool F_SPEED_L1 = FALSE;
bool F_SPEED_L2 = FALSE;
bool F_SPEED_R1 = FALSE;
bool F_SPEED_R2 = FALSE;

double tmp_minimum;
double tmp_expected;

void find_faulty()
{
    for (int i=0; i<NUM_POLL; i++)
    {
        // ACCEL Left
        tmp_minimum = fmin(ACCELERATION_L1[i], ACCELERATION_L2[i]);
        if (fabs(ACCELERATION_L1[i]-ACCELERATION_L2[i]) >= tmp_minimum * X) {
            tmp_expected = (SPEED_L1[i-1] - SPEED_L2[i]) / T;
            if (fabs(ACCELERATION_L1[i] - tmp_expected) > fabs(ACCELERATION_L2[i] - tmp_expected))
            {
                F_ACCELERATION_L1 = TRUE;
            } else
            {
                F_ACCELERATION_L2 = TRUE;
            }
        }
        // ACCEL Right
        tmp_minimum = fmin(ACCELERATION_R1[i], ACCELERATION_R2[i]);
        if (fabs(ACCELERATION_R1[i] - ACCELERATION_R2[i]) >= tmp_minimum * X) {
            tmp_expected = (SPEED_R1[i-1] - SPEED_R2[i]) / T;
            if (fabs(ACCELERATION_R1[i] - tmp_expected) > fabs(ACCELERATION_R2[i] - tmp_expected))
            {
                F_ACCELERATION_R1 = TRUE;
            } else
            {
                F_ACCELERATION_R2 = TRUE;
            }
        }
        // SPEED Left       // Assume the first value is never wrong since it is not possible to check with acceleration value without 2 time stamp.
        tmp_minimum = fmin(SPEED_L1[i], SPEED_L2[i]);
        if (fabs(SPEED_L1[i] - SPEED_L2[i]) >= tmp_minimum * X) {
            tmp_expected = ACCELERATION_L1[i];
            if ((fabs(SPEED_L1[i-1] - SPEED_L1[i]) / T) > (tmp_expected + tmp_expected * X) || (fabs(SPEED_L1[i-1] - SPEED_L1[i]) / T) < (tmp_expected + tmp_expected * X))
            {
                F_SPEED_L1 = TRUE;
            } else
            {
                F_SPEED_L2 = TRUE;
            }
        }
        // SPEED Right
        tmp_minimum = fmin(SPEED_R1[i], SPEED_R2[i]);
        if (fabs(SPEED_R1[i] - SPEED_R2[i]) >= tmp_minimum * X) {
            tmp_expected = ACCELERATION_R1[i];
            if ((fabs(SPEED_R1[i-1] - SPEED_R1[i]) / T) > (tmp_expected + tmp_expected * X) || (fabs(SPEED_R1[i-1] - SPEED_R1[i]) / T) < (tmp_expected + tmp_expected * X))
            {
                F_SPEED_R1 = TRUE;
            } else
            {
                F_SPEED_R2 = TRUE;
            }
        }
    }
}

int main()
{
    while(1) {
        for (int i=0; i<NUM_POLL; i++)
        {
            find_faulty();
            printf("iteration: %d   ", i);
            printf("%d", F_ACCELERATION_L1);
            printf("%d", F_ACCELERATION_L2);
            printf("%d", F_ACCELERATION_R1);
            printf("%d", F_ACCELERATION_R2);
            printf("%d", F_SPEED_L1);
            printf("%d", F_SPEED_L2);
            printf("%d", F_SPEED_R1);
            printf("%d\n", F_SPEED_R2);
        }
        break;
    }
    return 0;
}
