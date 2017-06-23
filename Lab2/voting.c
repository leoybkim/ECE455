#include "voting.h"
#include "fault_injection.h"
#include <math.h>
#include <stdio.h>



// Several types of voting systems exist, both for storage and for computations. For the
// purpose of this lab, you will implement a majority voting system for a redundantly computed value.
// In our case, you will implement the Newton-Raphson method (see Appendix) to compute the square
// root of a value (a double argument to the function). Client code will execute this sqrt function three
// times in succession, and accept the result if at least two of the computed results agree. In the case of
// double, agreement between two values means that the two values are within a given distance ϵ. For
// the purpose of this lab, we will consider ϵ = 10−6
// (1e-6 in C/C++); that is, agreement between x1
// and x2 means that | x1 − x2 | < 10−6
// )


double newton_raphson(double a) {
    double value = 1;
		int i;
    for (i = 0; i < ITERATION; i++) {
        value = (0.5)*(value + a/value);
    }
    // return faulty_double(value, RANDOM_FAULT);
    return value;
}

int voting_system(double a) {
    double trial_1;
    double trial_2;
    double trial_3;

    trial_1 = newton_raphson(faulty_double(a, RANDOM_FAULT));
    fault_injection_reset();
    trial_2 = newton_raphson(faulty_double(a, RANDOM_FAULT));
    fault_injection_reset();
    trial_3 = newton_raphson(faulty_double(a, RANDOM_FAULT));
    fault_injection_reset();
    // trial_1 = newton_raphson(a);
    // fault_injection_reset();
    // trial_2 = newton_raphson(a);
    // fault_injection_reset();
    // trial_3 = newton_raphson(a);
    // fault_injection_reset();

    if (!( (fabs(trial_1 - trial_2) < EPSILON) || (fabs(trial_2 - trial_3) < EPSILON)
     || (fabs(trial_1 - trial_3) < EPSILON) )){
        printf("%f, %f, %f\n", trial_1, trial_2, trial_3);
        return 1;
    } else {
        printf("%f, %f, %f\n", trial_1, trial_2, trial_3);
    }

    return 0;
}
