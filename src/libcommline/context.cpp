#include <commline/context.h>

namespace commline {
    auto print_version(std::ostream& os, const app& a) -> void {
        os << a.name << ' ' << a.version << '\n';
    }
}
