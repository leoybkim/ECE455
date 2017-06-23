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

// result #1
double c_sqrt(double data) {
    return sqrt(data);
}

// result #2 included in voting.h


// result #3 
double binary_search(double data) {
    // start = 1, mid = data/2 ,end = data
    // if mid*mid > end, choose bottom half
    // else choose top half
    // repeat until start > end

    double root = 0.0;
    double mid;
    double start = 0.0;
    double end = data;

    while(start <= end) {
        mid = (start + end) /2;
        root = mid;
        if (fabs((mid * mid) - data) <= EPSILON) {
            break;
        } else if ((mid * mid) < data) {
            start = mid;
        } else {
            end = mid;
        }
    }
    return root;
}

double heterogeneous (double a) {
    double result_1;
    double result_2;
    double result_3;

    result_1 = faulty_double(c_sqrt(a), RANDOM_FAULT);
    fault_injection_reset();
    result_2 = faulty_double(newton_raphson(a), RANDOM_FAULT);
    fault_injection_reset();
    result_3 = faulty_double(binary_search(a), RANDOM_FAULT);
    fault_injection_reset();

    if ( fabs(result_1 - result_2) < EPSILON || fabs(result_1 - result_3) < EPSILON) {
        return result_1;
    } else if (fabs(result_2 - result_3) < EPSILON) {
        return result_2;
    } else {
        printf("%f, %f, %f \n", result_1, result_2, result_3);
        return -1.0;
    }
}
