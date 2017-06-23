#include "heterogeneous.h"
#include "voting.h"
#include "fault_injection.h"
#include <stdio.h>
#include <math.h>

// This technique is similar to the previous one, but it introduces
// diversity: each of the three computations is executed with a different, independently implemented
// algorithm. In our case, we will compute result #1 using the C standard library sqrt function; result
// #2 using the Newton-Raphson method; and result #3 using binary search to narrow down the result
// to within 10−6 accuracy.
// The voting/decision protocol in this case is: if result #1 agrees with either one of the other results,
// then return result #1; if result #1 disagrees but results #2 and #3 agree, then return result #2. If all
// three values disagree, inform client code that a fault occurred (to this end, you could use a mechanism
// similar to the one described in item 1).

double c_sqrt(double data) {
    return sqrt(data);
}

double binary_search() {

    // TODO: complete function
    return 0;
}

double heterogeneous (double a) {
    double result_1;
    double result_2;
    double result_3;

    result_1 = fault_injection(c_sqrt(a), RANDOM_FAULT);
    result_2 = fault_injection(newton_raphson(a), RANDOM_FAULT);
    result_3 = fault_injection(binary_search(a), RANDOM_FAULT);

    if ( fabs(result_1 - result_2) < EPSILON || fabs(result_1 - result_3) < EPSILON) {
        return result_1;
    } else if (fabs(result_2 - result_3) < EPSILON) {
        return result_2;
    } else {
        printf("ERROR \n");
        return -1;
    }
}
