#include <iostream>
#include <cstddef>
#include "math/Matrix.h"
#include "math/Vector.h"

int main() {
    Matrix44f test;
    test[4] = 10.f;

    std::cout << test[4] << std::endl;

    Vector<int, 2> v({1, 2});
    std::cout << v.y() << std::endl;

    return 0;
}