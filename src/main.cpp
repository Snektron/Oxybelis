#include <iostream>
#include <type_traits>
#include "math/Matrix.h"
//#include "math/Vector.h"

int main() {
    const Matrix<float, 2, 2> a({1.f, 2.f, 3.f, 4.f});
    Matrix<int, 2, 2> b({1, 2, 3, 4});

    std::cout << (a + b)(1, 0) << std::endl;

    return 0;
}