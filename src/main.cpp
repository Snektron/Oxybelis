#include <iostream>
#include <type_traits>
#include "math/Matrix.h"
#include "math/Vector.h"

int main() {
    Matrix<float, 2, 2> a({1, 2, 3, 4});
    Matrix<float, 2, 2> b({5, 6, 7, 8});

    a.negate();

    std::cout << a << b << (a * b) << std::endl;

    return 0;
}