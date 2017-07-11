#include <stdio.h>
#include <math.h>
#include "sensors.h"

typedef int bool;
#define TRUE 1
#define FALSE 0
// #define min(x,y) ((x) <= (y)) ? (x) : (y)


// double ACCELERATION_L1[] = { 0.0, -2.5, -1.0, -1.0, -5.5, -8.0, -4.0, -3.0, -1.54, -0.46, -4.0, -3.0};
// double ACCELERATION_L2[] = { 0.0, -2.5, -1.0, -1.0, -5.5, -8.0, -4.0, -3.0, -1.54, -0.46, -4.0, -3.0};
// double ACCELERATION_R1[] = { 0.0, -2.5, -1.0, -1.0, -5.5, -8.0, -4.0, -3.0, -1.54, -0.46, -4.0, -3.0};
// double ACCELERATION_R2[] = { 0.0, -2.5, -1.0, -1.0, -5.5, -8.0, -4.0, -3.0, -1.54, -0.46, -4.0, -3.0};
// double SPEED_L1[]        = {50.0, 47.5, 46.5, 45.5, 40.0, 32.0, 28.0, 11.0,  9.46,   9.0,  5.0,  2.0};
// double SPEED_L2[]        = {50.0, 47.5, 46.5, 45.5, 40.0, 32.0, 28.0, 11.0,  9.46,   9.0,  5.0,  2.0};
// double SPEED_R1[]        = {50.0, 47.5, 46.5, 45.5, 40.0, 32.0, 28.0, 11.0,  9.46,   9.0,  5.0,  2.0};
// double SPEED_R2[]        = {50.0, 47.5, 46.5, 45.5, 40.0, 32.0, 28.0, 11.0,  9.46,   9.0,  5.0,  2.0};

// TEST 0) No faults, No collision
// double ACCELERATION_L1[] = { 0.0, -2.5, -1.0, -1.0, -2.5, 2.0, -3.5, -3.5,  1.46, -0.44, -3.0, -1.0};
// double ACCELERATION_L2[] = { 0.0, -2.5, -1.0, -1.0, -2.5, 2.0, -3.5, -3.5,  1.46, -0.44, -3.0, -1.0};
// double ACCELERATION_R1[] = { 0.0, -2.5, -1.0, -1.0, -2.5, 2.0, -3.5, -3.5,  1.46, -0.44, -3.0, -1.0};
// double ACCELERATION_R2[] = { 0.0, -2.5, -1.0, -1.0, -2.5, 2.0, -3.5, -3.5,  1.46, -0.44, -3.0, -1.0};
// double SPEED_L1[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46, 39.0,  36.0,  35.0};
// double SPEED_L2[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46, 39.0,  36.0,  35.0};
// double SPEED_R1[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46, 39.0,  36.0,  35.0};
// double SPEED_R2[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46, 39.0,  36.0,  35.0};

// TEST 1) No faults; Frontal Collision at index 4
// double ACCELERATION_L1[] = { 0.0, -2.5, -1.0, -1.0, -5.5, -8.0, -4.0, -3.0, -1.54, -0.46, -4.0, -3.0};
// double ACCELERATION_L2[] = { 0.0, -2.5, -1.0, -1.0, -5.5, -8.0, -4.0, -3.0, -1.54, -0.46, -4.0, -3.0};
// double ACCELERATION_R1[] = { 0.0, -2.5, -1.0, -1.0, -5.5, -8.0, -4.0, -3.0, -1.54, -0.46, -4.0, -3.0};
// double ACCELERATION_R2[] = { 0.0, -2.5, -1.0, -1.0, -5.5, -8.0, -4.0, -3.0, -1.54, -0.46, -4.0, -3.0};
// double SPEED_L1[]        = {50.0, 47.5, 46.5, 45.5, 40.0, 32.0, 28.0, 11.0,  9.46,   9.0,  5.0,  2.0};
// double SPEED_L2[]        = {50.0, 47.5, 46.5, 45.5, 40.0, 32.0, 28.0, 11.0,  9.46,   9.0,  5.0,  2.0};
// double SPEED_R1[]        = {50.0, 47.5, 46.5, 45.5, 40.0, 32.0, 28.0, 11.0,  9.46,   9.0,  5.0,  2.0};
// double SPEED_R2[]        = {50.0, 47.5, 46.5, 45.5, 40.0, 32.0, 28.0, 11.0,  9.46,   9.0,  5.0,  2.0};

// TEST 2) Faulty; No collision
// faulty @ ACCELERATION_L1[3] -- ACCELERATION_L2[4] -- ACCELERATION_R1[5] -- ACCELERATION_R2[6]
//          SPEED_L1[7]        -- SPEED_L2[8]        -- SPEED_R1[9]        -- SPEED_R2[10]
// double ACCELERATION_L1[] = { 0.000, -2.500, -1.005, -1.100, -2.500,  2.000, -3.500, -3.500,  1.460,-0.4400,  -3.000,  -1.000};
// double ACCELERATION_L2[] = { 0.000, -2.500, -1.000, -1.000, -2.400,  2.000, -3.500, -3.500,  1.460,-0.4400,  -3.000,  -1.000};
// double ACCELERATION_R1[] = { 0.000, -2.500, -1.000, -1.000, -2.500,  2.500, -3.500, -3.500,  1.460,-0.4400,  -3.000,  -1.000};
// double ACCELERATION_R2[] = { 0.000, -2.500, -1.000, -1.000, -2.500,  2.000, -5.500, -3.500,  1.460,-0.4400,  -3.000,  -1.000};
// double SPEED_L1[]        = {50.000, 47.500, 46.500, 45.500, 43.000, 45.000, 41.500, 50.000, 39.460, 39.000,  36.000,  35.000};
// double SPEED_L2[]        = {50.000, 47.504, 46.501, 45.500, 43.000, 45.000, 41.500, 38.000, 30.460, 39.000,  36.000,  35.000};
// double SPEED_R1[]        = {50.000, 47.500, 46.499, 45.500, 43.000, 45.000, 41.500, 38.000, 39.460, 31.000,  36.000,  35.000};
// double SPEED_R2[]        = {50.000, 47.532, 46.487, 45.500, 43.000, 45.000, 41.500, 38.000, 39.460, 39.000,  20.000,  35.000};




// TEST 4) doesn't meet CORRELATION_THRESHOLD at index 5 --> shouldn't be collision
// double ACCELERATION_L1[] = { 0.000,  -2.501, -1.000, -1.001, -5.498, -8.000, -4.000, -3.000, -1.540, -0.460, -4.000, -3.000};
// double ACCELERATION_L2[] = { 0.000,  -2.499, -1.000, -1.000, -5.500, -8.000, -4.000, -3.000, -1.540, -0.460, -4.000, -3.000};
// double ACCELERATION_R1[] = { 0.000,  -2.502, -1.000, -1.002, -5.496, -5.000, -7.000, -3.000, -1.540, -0.460, -4.000, -3.000};
// double ACCELERATION_R2[] = { 0.000,  -2.500, -1.000, -1.000, -5.500, -8.000, -4.000, -3.000, -1.540, -0.460, -4.000, -3.000};
// double SPEED_L1[]        = {50.000,  47.501, 46.501, 45.502, 40.000, 32.000, 28.000, 11.000,  9.460,  9.000,  5.000,  2.000};
// double SPEED_L2[]        = {50.000,  47.499, 46.499, 45.500, 40.000, 32.000, 28.000, 11.000,  9.460,  9.000,  5.000,  2.000};
// double SPEED_R1[]        = {50.000,  47.502, 46.502, 45.504, 40.000, 35.000, 28.000, 11.000,  9.460,  9.000,  5.000,  2.000};
// double SPEED_R2[]        = {50.000,  47.500, 46.500, 45.500, 40.000, 32.000, 28.000, 11.000,  9.460,  9.000,  5.000,  2.000};



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
    // printf("\n");

    if (iter > 0)
    {
        // ACCEL Left
        tmp_minimum = fmin(fabs(ACCELERATION_L1[iter]), fabs(ACCELERATION_L2[iter]));
        if (fabs(ACCELERATION_L1[iter] - ACCELERATION_L2[iter]) >= tmp_minimum * X)
        {
            tmp_expected = (SPEED_L1[iter] - SPEED_L2[iter-1]) / T;
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
        tmp_minimum = fmin(fabs(ACCELERATION_R1[iter]), fabs(ACCELERATION_R2[iter]));
        if (fabs(ACCELERATION_R1[iter] - ACCELERATION_R2[iter]) >= tmp_minimum * X)
        {
            tmp_expected = (SPEED_R1[iter] - SPEED_R2[iter-1]) / T;
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
            if ((fabs((SPEED_L1[iter] - SPEED_L1[iter-1])/T) - tmp_expected) > (fabs((SPEED_L2[iter] -SPEED_L2[iter-1])/T) - tmp_expected))
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
            if ((fabs((SPEED_R1[iter] - SPEED_R1[iter-1])/T) - tmp_expected) > (fabs((SPEED_R2[iter] -SPEED_R2[iter-1])/T) - tmp_expected))
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
                if ((fabs(ACCELERATION_L1[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_R1[iter]) > ACCELERATION_THRESHOLD)
                    || (fabs(ACCELERATION_L1[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_R1[iter]) > CORRELATION_THRESHOLD * ACCELERATION_L1[iter])
                    || (fabs(ACCELERATION_R1[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_L1[iter]) > CORRELATION_THRESHOLD * ACCELERATION_R1[iter]))
                {
                    return TRUE;
                }
            }
            else if (!F_ACCELERATION_R2)
            {
                if ((fabs(ACCELERATION_L1[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_R2[iter]) > ACCELERATION_THRESHOLD)
                    || (fabs(ACCELERATION_L1[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_R2[iter]) > CORRELATION_THRESHOLD * ACCELERATION_L1[iter])
                    || (fabs(ACCELERATION_R2[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_L1[iter]) > CORRELATION_THRESHOLD * ACCELERATION_R2[iter]))
                {
                    return TRUE;
                }
            }
        }
        else if (!F_ACCELERATION_L2)
        {
            if(!F_ACCELERATION_R1)
            {
                if ((fabs(ACCELERATION_L2[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_R1[iter]) > ACCELERATION_THRESHOLD)
                    || (fabs(ACCELERATION_L2[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_R1[iter]) > CORRELATION_THRESHOLD * ACCELERATION_L2[iter])
                    || (fabs(ACCELERATION_R1[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_L2[iter]) > CORRELATION_THRESHOLD * ACCELERATION_R1[iter]))
                {
                    return TRUE;
                }
            }
            else if (!F_ACCELERATION_R2)
            {
                if ((fabs(ACCELERATION_L2[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_R2[iter]) > ACCELERATION_THRESHOLD)
                    || (fabs(ACCELERATION_L2[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_R2[iter]) > CORRELATION_THRESHOLD * ACCELERATION_L2[iter])
                    || (fabs(ACCELERATION_R2[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_L2[iter]) > CORRELATION_THRESHOLD * ACCELERATION_R2[iter]))
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
        for (int i=0; i<NUM_POLL; i++)
        {
            if(collision)
            {
                break;
            }
            else
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
                if (collision)
                {
                    printf("Collision detected! at index %d \n", i);
                }
                // reset faulty flags for sensors
                reset();
            }
        }
    }
    return 0;
}
