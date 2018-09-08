#include <iostream>
#include <cstddef>
#include <string_view>

extern const char resource_a_oof_txt[];
extern size_t resource_a_oof_txt_size;

int main() {
    std::string_view oof(resource_a_oof_txt, resource_a_oof_txt_size);
    std::cout << "Text: " << oof << std::endl;
    return 0;
}