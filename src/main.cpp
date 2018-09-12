#include <iostream>
#include <type_traits>
#include "math/Matrix.h"
#include "math/Vector.h"

int main() {
    Vector<double, 3> c({3, -3, 1});
    Vector<double, 3> d({4, 9, 2});

    c.cross(d);

    auto e = -(c * 10);

    std::cout << e << std::endl;

    return 0;
}