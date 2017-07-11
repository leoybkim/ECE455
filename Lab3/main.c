#include <stdio.h>
#include <lpc17xx.h>
#include <math.h>
#include <string.h>
//#include <stdlib.h>
#include "sensors.h"
#include "glcd.h"
#include "inputs.h"

typedef int bool;
#define TRUE  1
#define FALSE 0
#define fmin(x,y) ((x) <= (y)) ? (x) : (y)


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

// TEST 2) Faulty ACCELERATION_R1; No collision
// double ACCELERATION_L1[] = { 0.0, -2.5, -1.0, -1.0, -2.5,  2.0, -3.5, -3.5,  1.46, -0.44, -3.0, -1.0};
// double ACCELERATION_L2[] = { 0.0, -2.5, -1.0, -1.0, -2.5,  2.0, -3.5, -3.5,  1.46, -0.44, -3.0, -1.0};
// double ACCELERATION_R1[] = { 0.1, -2.9, -1.0, -1.0,  2.5,  2.0, -3.5,-12.0,  1.46, -0.10, -3.0, -10.0};
// double ACCELERATION_R2[] = { 0.0, -2.5, -1.0, -1.0, -2.5,  2.0, -3.5, -3.5,  1.46, -0.44, -3.0, -1.0};
// double SPEED_L1[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46,  39.0,  36.0,  35.0};
// double SPEED_L2[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46,  39.0,  36.0,  35.0};
// double SPEED_R1[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46,  39.0,  36.0,  35.0};
// double SPEED_R2[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46,  39.0,  36.0,  35.0};

// TEST 3) Faulty ACCELERATION_R1; Frontal collision at index 10
// double ACCELERATION_L1[] = { 0.0, -2.5, -1.0, -1.0, -2.5,  2.0, -3.5, -3.5,  1.46, -0.44, -4.2,    0.2};
// double ACCELERATION_L2[] = { 0.0, -2.5, -1.0, -1.0, -2.5,  2.0, -3.5, -3.5,  1.46, -0.44, -4.2,    0.2};
// double ACCELERATION_R1[] = { 0.1, -2.9, -1.0, -1.0,  2.5,  2.0, -3.5,-12.0,  1.46, -0.10, -2.0,  -10.0};
// double ACCELERATION_R2[] = { 0.0, -2.5, -1.0, -1.0, -2.5,  2.0, -3.5, -3.5,  1.46, -0.44, -4.2,    0.2};
// double SPEED_L1[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46,  39.0,  34.8,  35.0};
// double SPEED_L2[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46,  39.0,  34.8,  35.0};
// double SPEED_R1[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46,  39.0,  34.8,  35.0};
// double SPEED_R2[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46,  39.0,  34.8,  35.0};


// TEST 4) Faulty ACCELERATION_R1; No Collision due to CORRELATION_THRESHOLD at index 10
// double ACCELERATION_L1[] = { 0.0, -2.5, -1.0, -1.0, -2.5,  2.0, -3.5, -3.5,  1.46, -0.44,  -4.2,   0.2};
// double ACCELERATION_L2[] = { 0.0, -2.5, -1.0, -1.0, -2.5,  2.0, -3.5, -3.5,  1.46, -0.44,  -4.2,   0.2};
// double ACCELERATION_R1[] = { 0.1, -2.9, -1.0, -1.0,  2.5,  2.0, -3.5,-12.0,  1.46, -0.10,  -2.0, -10.0};
// double ACCELERATION_R2[] = { 0.0, -2.5, -1.0, -1.0, -2.5,  2.0, -3.5, -3.5,  1.46, -0.44,  -1.5,  -2.5};
// double SPEED_L1[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46,  39.0,  34.8,  35.0};
// double SPEED_L2[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46,  39.0,  34.8,  35.0};
// double SPEED_R1[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46,  39.0,  37.5,  35.0};
// double SPEED_R2[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46,  39.0,  37.5,  35.0};

// TEST 5) Faulty ACCELERATION_R1; Collision at index 10; Passes CORRELATION_THRESHOLD;
// double ACCELERATION_L1[] = { 0.0, -2.5, -1.0, -1.0, -2.5,  2.0, -3.5, -3.5,  1.46, -0.44,  -4.2,   0.2};
// double ACCELERATION_L2[] = { 0.0, -2.5, -1.0, -1.0, -2.5,  2.0, -3.5, -3.5,  1.46, -0.44,  -4.2,   0.2};
// double ACCELERATION_R1[] = { 0.1, -2.9, -1.0, -1.0,  2.5,  2.0, -3.5,-12.0,  1.46, -0.10,  -2.0,  -1.0};
// double ACCELERATION_R2[] = { 0.0, -2.5, -1.0, -1.0, -2.5,  2.0, -3.5, -3.5,  1.46, -0.44,  -3.0,  -1.0};
// double SPEED_L1[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46,  39.0,  34.8,  35.0};
// double SPEED_L2[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46,  39.0,  34.8,  35.0};
// double SPEED_R1[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46,  39.0,  36.0,  35.0};
// double SPEED_R2[]        = {50.0, 47.5, 46.5, 45.5, 43.0, 45.0, 41.5, 38.0, 39.46,  39.0,  36.0,  35.0};

// TEST 6) NOT Faulty ACCELERATION_R1 because within X%; Collision at index 10; Passes CORRELATION_THRESHOLD;
double ACCELERATION_L1[] = { 0.0, -2.5 , -1.0, -1.0, -2.5,  2.0, -3.5, -3.5 ,  1.46, -0.44,  -4.2 ,   0.2};
double ACCELERATION_L2[] = { 0.0, -2.5 , -1.0, -1.0, -2.5,  2.0, -3.5, -3.5 ,  1.46, -0.44,  -4.2 ,   0.2};
double ACCELERATION_R1[] = { 0.0, -2.51, -1.0, -1.0, -2.5,  2.0, -3.5, -3.53,  1.46, -0.44,  -2.98,  -1.0};
double ACCELERATION_R2[] = { 0.0, -2.5 , -1.0, -1.0, -2.5,  2.0, -3.5, -3.5 ,  1.46, -0.44,  -3.0 ,  -1.0};
double SPEED_L1[]        = {50.0, 47.5 , 46.5, 45.5, 43.0, 45.0, 41.5, 38.0 , 39.46,  39.0,  34.8 ,  35.0};
double SPEED_L2[]        = {50.0, 47.5 , 46.5, 45.5, 43.0, 45.0, 41.5, 38.0 , 39.46,  39.0,  34.8 ,  35.0};
double SPEED_R1[]        = {50.0, 47.5 , 46.5, 45.5, 43.0, 45.0, 41.5, 38.0 , 39.46,  39.0,  36.0 ,  35.0};
double SPEED_R2[]        = {50.0, 47.5 , 46.5, 45.5, 43.0, 45.0, 41.5, 38.0 , 39.46,  39.0,  36.0 ,  35.0};


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
                    || (fabs(ACCELERATION_L1[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_R1[iter]) > CORRELATION_THRESHOLD * fabs(ACCELERATION_L1[iter]))
                    || (fabs(ACCELERATION_R1[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_L1[iter]) > CORRELATION_THRESHOLD * fabs(ACCELERATION_R1[iter])))
                {
                    return TRUE;
                }
            }
            else if (!F_ACCELERATION_R2)
            {
                if ((fabs(ACCELERATION_L1[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_R2[iter]) > ACCELERATION_THRESHOLD)
                    || (fabs(ACCELERATION_L1[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_R2[iter]) > CORRELATION_THRESHOLD * fabs(ACCELERATION_L1[iter]))
                    || (fabs(ACCELERATION_R2[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_L1[iter]) > CORRELATION_THRESHOLD * fabs(ACCELERATION_R2[iter])))
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
                    || (fabs(ACCELERATION_L2[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_R1[iter]) > CORRELATION_THRESHOLD * fabs(ACCELERATION_L2[iter]))
                    || (fabs(ACCELERATION_R1[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_L2[iter]) > CORRELATION_THRESHOLD * fabs(ACCELERATION_R1[iter])))
                {
                    return TRUE;
                }
            }
            else if (!F_ACCELERATION_R2)
            {
                if ((fabs(ACCELERATION_L2[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_R2[iter]) > ACCELERATION_THRESHOLD)
                    || (fabs(ACCELERATION_L2[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_R2[iter]) > CORRELATION_THRESHOLD * fabs(ACCELERATION_L2[iter]))
                    || (fabs(ACCELERATION_R2[iter]) > ACCELERATION_THRESHOLD && fabs(ACCELERATION_L2[iter]) > CORRELATION_THRESHOLD * fabs(ACCELERATION_R2[iter])))
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

// initial board setup
void init_setup() 
{
    SystemInit();
    GLCD_Init();    
    GLCD_Clear(White);  
}

int main()
{
    // debug tags
    char str_test[20];
        
    int i;
    
    // initialize
    init_setup();
   
    // testing
    strcpy(str_test, "Hello World");
    GLCD_DisplayString(0, 0, 1, (unsigned char*) str_test);

    while(1)
    {
        for (i=0; i<NUM_POLL; i++)
        {
            if(collision)
            {
                break;
            }
            else
            {
                find_faulty(i);
                /*printf("iteration: %d   ", i);
                printf("%d", F_ACCELERATION_L1);
                printf("%d", F_ACCELERATION_L2);
                printf("%d", F_ACCELERATION_R1);
                printf("%d", F_ACCELERATION_R2);
                printf("%d", F_SPEED_L1);
                printf("%d", F_SPEED_L2);
                printf("%d", F_SPEED_R1);
                printf("%d\n", F_SPEED_R2);*/
                collision = detect_collision(i);
                if (collision)
                {
                    //printf("Collision detected! at index %d \n", i);
                }
                // reset faulty flags for sensors
                reset();
            }
        }
    }
}
