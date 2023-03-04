#include <commline/application.h>
#include <fmt/core.h>

namespace commline {
    auto print_error(std::exception_ptr eptr) -> void {
        try {
            if (eptr) std::rethrow_exception(eptr);
        }
        catch (const std::exception& ex) {
            fmt::print(stderr, "{}\n", ex.what());
        }
    }
}
