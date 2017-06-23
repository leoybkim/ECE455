#include "verification.h"
#include "voting.h"
#include "fault_injection.h"
#include <stdio.h>
#include <math.h>

// With this technique, we verify the result of a (possibly complex and thus
// potentially error-prone) computation by applying the inverse operation or some result confirmation
// operation. In our case, we will work with the Newton-Raphson implementation of the square root
// function, and we verify the value returned by that function by computing its square and checking
// whether it agrees with the input value (the one for which we computed the square root).

int inverse(double data) {
    double nr_return_value = faulty_double(newton_raphson(data), RANDOM_FAULT);
    if (fabs((nr_return_value * nr_return_value) - data) < EPSILON) {
        printf("%f, %f\n", nr_return_value, data);
        return 0;
    } else {
        printf("%f, %f\n", nr_return_value, data);
        return 1;
    }
}
