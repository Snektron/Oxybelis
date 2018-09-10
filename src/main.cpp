#include <iostream>
#include <type_traits>
#include "math/Matrix.h"
//#include "math/Vector.h"

int main() {
    Matrix<float, 4, 4> test;

    std::cout << test.rows() << ' ' << test.Rows << std::endl;

    return 0;
}