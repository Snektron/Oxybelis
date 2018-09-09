#include <iostream>
#include <cstddef>
#include <string_view>
#include "resources.h"

int main() {
    std::string_view oof(resource_a_oof_txt, resource_a_oof_txt_size);
    std::cout << "Text: " << oof << std::endl;
    std::cout << "Size: " << resource_a_oof_txt_size << std::endl;
    return 0;
}