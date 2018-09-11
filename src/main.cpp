#include <iostream>
#include <type_traits>
#include "math/Matrix.h"
//#include "math/Vector.h"

int main() {
    Matrix<float, 2, 2> test({1.f, 2.f, 3.f, 4.5f});

    std::cout << test.rows() << ' ' << test(1, 1) << std::endl;

    return 0;
}