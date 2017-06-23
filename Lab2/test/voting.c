#include "voting.h"
#include "fault_injection.h"

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


double newton_raphson(double a, int iter) {
    double value = 1;
    for (int i = 0; i < iter; i++) {
        value = (0.5)*(value + a/value);
    }
    //return faulty_double(value, RANDOM_FAULT);
    return faulty_double(value, RANDOM_FAULT);
}
