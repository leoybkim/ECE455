#include <stdio.h>
#include <math.h>
#include "sensors.h"

typedef int bool;
#define TRUE 1
#define FALSE 0
// #define min(x,y) ((x) <= (y)) ? (x) : (y)

double ACCELERATION_L1[] = {0.0, 5.0, 5.0, 3.0, 2.0, 2.0, 1.0, 1.0, 1.0, 8.0, 8.0, 5.0, 5.0, 2.0};
double ACCELERATION_L2[] = {0.0, 5.0, 5.0, 3.0, 2.0, 2.0, 1.0, 1.0, 1.0, 8.0, 8.0, 5.0, 5.0, 2.0};
double ACCELERATION_R1[] = {0.0, 5.0, 5.0, 3.0, 2.0, 2.0, 1.0, 1.0, 1.0, 8.0, 8.0, 5.0, 5.0, 2.0};
double ACCELERATION_R2[] = {0.0, 5.0, 5.0, 3.0, 2.0, 2.0, 1.0, 1.0, 1.0, 8.0, 8.0, 5.0, 5.0, 2.0};
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

bool collision = FALSE;

void find_faulty(int iter)
{
    printf("\n");

    if (iter > 0)
    {
        // ACCEL Left
        tmp_minimum = fmin(ACCELERATION_L1[iter], ACCELERATION_L2[iter]);
        //printf("tmp_min: %f  diff: %f \n", tmp_minimum * X, fabs(ACCELERATION_L1[i] - ACCELERATION_L2[i]));

        if (fabs(ACCELERATION_L1[iter] - ACCELERATION_L2[iter]) >= tmp_minimum * X)
        {
            tmp_expected = (SPEED_L1[iter-1] - SPEED_L2[iter]) / T;
            if (fabs(ACCELERATION_L1[iter] - tmp_expected) > fabs(ACCELERATION_L2[iter] - tmp_expected))
            {
                F_ACCELERATION_L1 = TRUE;
            }
            else
            {
                F_ACCELERATION_L2 = TRUE;
            }
        }
        // ACCEL Right
        tmp_minimum = fmin(ACCELERATION_R1[iter], ACCELERATION_R2[iter]);
        if (fabs(ACCELERATION_R1[iter] - ACCELERATION_R2[iter]) >= tmp_minimum * X) {
            tmp_expected = (SPEED_R1[iter-1] - SPEED_R2[iter]) / T;
            if (fabs(ACCELERATION_R1[iter] - tmp_expected) > fabs(ACCELERATION_R2[iter] - tmp_expected))
            {
                F_ACCELERATION_R1 = TRUE;
            }
            else
            {
                F_ACCELERATION_R2 = TRUE;
            }
        }
        // SPEED Left       // Assume the first value is never wrong since it is not possible to check with acceleration value without 2 time stamp.
        tmp_minimum = fmin(SPEED_L1[iter], SPEED_L2[iter]);
        if (fabs(SPEED_L1[iter] - SPEED_L2[iter]) >= tmp_minimum * X)
        {
            tmp_expected = ACCELERATION_L1[iter];
            if ((fabs(SPEED_L1[iter-1] - SPEED_L1[iter]) / T) > (tmp_expected + tmp_expected * X) || (fabs(SPEED_L1[iter-1] - SPEED_L1[iter]) / T) < (tmp_expected + tmp_expected * X))
            {
                F_SPEED_L1 = TRUE;
            } else
            {
                F_SPEED_L2 = TRUE;
            }
        }
        // SPEED Right
        tmp_minimum = fmin(SPEED_R1[iter], SPEED_R2[iter]);
        if (fabs(SPEED_R1[iter] - SPEED_R2[iter]) >= tmp_minimum * X)
        {
            tmp_expected = ACCELERATION_R1[iter];
            if ((fabs(SPEED_R1[iter-1] - SPEED_R1[iter]) / T) > (tmp_expected + tmp_expected * X) || (fabs(SPEED_R1[iter-1] - SPEED_R1[iter]) / T) < (tmp_expected + tmp_expected * X))
            {
                F_SPEED_R1 = TRUE;
            }
            else
            {
                F_SPEED_R2 = TRUE;
            }
        }
    }
}

bool detect_collision(int iter)
{
    // check SPEED_THRESHOLD on both side
    // by default, check the first sensor ex) SPEED_L1, SPEED_R1
    // if default sensor is faulty, checks redundant sensor ex) SPEED_L2, SPEED_R2
    if (((!F_SPEED_L1 && SPEED_L1[iter] > SPEED_THRESHOLD) || (!F_SPEED_L2 && SPEED_L2[iter] > SPEED_THRESHOLD))
    && (!F_SPEED_R1 && SPEED_R1[iter] > SPEED_THRESHOLD) || (!F_SPEED_R2 && SPEED_R2[iter] > SPEED_THRESHOLD))
    {

        // check ACCELERATION_THRESHOLD on both side
        // if one side fails threshold check, also check to see if difference between them matches the CORRELATION_THRESHOLD
        // by default, check the first sensor ex) ACCELERATION_L1, ACCELERATION_R1
        // if defalut sensor is faulty, checks redundant sensor ex) ACCELERATION_L2, ACCELERATION_R2
        if(!F_ACCELERATION_L1)
        {
            if(!F_ACCELERATION_R1)
            {
                if ((ACCELERATION_L1[iter] > ACCELERATION_THRESHOLD && ACCELERATION_R1[iter] > ACCELERATION_THRESHOLD)
                    || (ACCELERATION_L1[iter] > ACCELERATION_THRESHOLD && ACCELERATION_R1[iter] > CORRELATION_THRESHOLD * ACCELERATION_L1[iter])
                    || (ACCELERATION_R1[iter] > ACCELERATION_THRESHOLD && ACCELERATION_L1[iter] > CORRELATION_THRESHOLD * ACCELERATION_R1[iter]))
                {
                    return TRUE;
                }
            }
            else if (!F_ACCELERATION_R2)
            {
                if ((ACCELERATION_L1[iter] > ACCELERATION_THRESHOLD && ACCELERATION_R2[iter] > ACCELERATION_THRESHOLD)
                    || (ACCELERATION_L1[iter] > ACCELERATION_THRESHOLD && ACCELERATION_R2[iter] > CORRELATION_THRESHOLD * ACCELERATION_L1[iter])
                    || (ACCELERATION_R2[iter] > ACCELERATION_THRESHOLD && ACCELERATION_L1[iter] > CORRELATION_THRESHOLD * ACCELERATION_R2[iter]))
                {
                    return TRUE;
                }
            }
        }
        else if (!F_ACCELERATION_L2)
        {
            if(!F_ACCELERATION_R1)
            {
                if ((ACCELERATION_L2[iter] > ACCELERATION_THRESHOLD && ACCELERATION_R1[iter] > ACCELERATION_THRESHOLD)
                    || (ACCELERATION_L2[iter] > ACCELERATION_THRESHOLD && ACCELERATION_R1[iter] > CORRELATION_THRESHOLD * ACCELERATION_L2[iter])
                    || (ACCELERATION_R1[iter] > ACCELERATION_THRESHOLD && ACCELERATION_L2[iter] > CORRELATION_THRESHOLD * ACCELERATION_R1[iter]))
                {
                    return TRUE;
                }
            }
            else if (!F_ACCELERATION_R2)
            {
                if ((ACCELERATION_L2[iter] > ACCELERATION_THRESHOLD && ACCELERATION_R2[iter] > ACCELERATION_THRESHOLD)
                    || (ACCELERATION_L2[iter] > ACCELERATION_THRESHOLD && ACCELERATION_R2[iter] > CORRELATION_THRESHOLD * ACCELERATION_L2[iter])
                    || (ACCELERATION_R2[iter] > ACCELERATION_THRESHOLD && ACCELERATION_L2[iter] > CORRELATION_THRESHOLD * ACCELERATION_R2[iter]))
                {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

void reset()
{
    F_ACCELERATION_L1 = FALSE;
    F_ACCELERATION_L2 = FALSE;
    F_ACCELERATION_R1 = FALSE;
    F_ACCELERATION_R2 = FALSE;
    F_SPEED_L1 = FALSE;
    F_SPEED_L2 = FALSE;
    F_SPEED_R1 = FALSE;
    F_SPEED_R2 = FALSE;
}

int main()
{
    while(1)
    {
        if (!collision)
        {
            for (int i=0; i<NUM_POLL; i++)
            {
                find_faulty(i);
                printf("iteration: %d   ", i);
                printf("%d", F_ACCELERATION_L1);
                printf("%d", F_ACCELERATION_L2);
                printf("%d", F_ACCELERATION_R1);
                printf("%d", F_ACCELERATION_R2);
                printf("%d", F_SPEED_L1);
                printf("%d", F_SPEED_L2);
                printf("%d", F_SPEED_R1);
                printf("%d\n", F_SPEED_R2);

                collision = detect_collision(i);
                // reset faulty flags for sensors
                reset();
            }
        }
        break;
    }
    return 0;
}
