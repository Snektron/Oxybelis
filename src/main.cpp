#include <iostream>
#include <cstddef>
#include <string_view>
#include "assets.h"

int main() {
    auto& res = assets::a::oof_txt;
    std::string_view oof(res.data, res.size);
    std::cout << "Text: " << oof << std::endl;
    std::cout << "Size: " << res.size << std::endl;
    return 0;
}