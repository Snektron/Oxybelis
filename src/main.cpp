#include <iostream>
#include <type_traits>
#include "math/Matrix.h"
#include "math/Vector.h"

int main() {
    Vector<double, 4> v({1, 2, 3, 1});
    Vector<double, 3> t({10, 20, 30});

    v = make_scaling(t) * v;

    std::cout << t << std::endl << v << std::endl;
    return 0;
}