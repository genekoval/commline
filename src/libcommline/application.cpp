#include <commline/application.h>

namespace commline {
    auto print_error(const std::exception& ex) -> void {
        std::cerr << ex.what() << std::endl;
    }
}
