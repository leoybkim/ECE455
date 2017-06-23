#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "fault_injection.h"
#include "heterogeneous.h"
#include "redundancy.h"
#include "voting.h"
#include "verification.h"


// global variables
int test_val[] = {1, 100, 10000, 99999, 12345678};
integer_data original_int;
double_data original_double;


int main() {
    // debug tags
    char str_faulty_int[12];
    char str_original_int[12];
    char str_error[12];
    int faulty_int_var;

    int mode = 4;
    int type = 1;

    double first;
    double second;
    double third;


    double answer;

    // LEFT and RIGHT joystick for switching mode
    // UP and DOwn joystick for switching fault typedef
    // Press for INT/Double toggle

    while(1) {

        switch(mode){
            case 1:
                write_integer(test_val[3], &original_int);
                if (read_integer(&original_int)){
                   printf("error occured \n");
                   fault_injection_reset();
                } else {
                    printf("fine\n");
                }

                // TODO: fix!!
                write_double(100.00, &original_double);
                if (read_double(&original_double)){
                   printf("error occured \n");
                   fault_injection_reset();
                } else {
                   printf("fine\n");
                }

                break;
            case 2:
                // first = faulty_double(newton_raphson(100.00, 50), RANDOM_FAULT);
                // second = faulty_double(newton_raphson(100.00, 50), RANDOM_FAULT);
                // third = faulty_double(newton_raphson(100.00, 50), RANDOM_FAULT);
                //
                // // TODO: less then epsilon
                // // TODO: iter should be a constant
                // if (!( (abs(first - second) < 1e-6) || (abs(second - third) < 1e-6) || (abs(first - third) < 1e-6) )){
                //     printf("%f, %f, %f\n", first, second, third);
                //     //fault_injection_reset();
                // }
                // printf("%f, %f, %f\n", first, second, third);
                if (voting_system(100.00)) {
                    printf("ERROR \n");
                } else {
                    printf("FINE\n");
                }
                break;
            case 3:
                answer = heterogeneous(100.00);
                if (answer == -1.0) {
                    printf("ERROR \n");
                } else {
                    printf("FINE %f \n", answer);
                }
                break;
            case 4:
                if (inverse(10000.00)) {
                    printf("ERROR \n");
                } else {
                    printf("FINE \n");
                }
                break;
            default:
                break;
        }

        sleep(1);
    }


    return 0;
}
