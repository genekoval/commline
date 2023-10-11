#include <commline/print.h>

namespace {
    constexpr auto indent_size = 4;
}

namespace commline::print {
    auto header(std::ostream& out, std::string_view text) -> void {
        out << "\n" << text << ":\n";
    }

    auto indent(std::ostream& out) -> void { spaces(out, indent_size); }

    auto spaces(std::ostream& out, int amount) -> void {
        for (auto i = 0; i < amount; ++i) out << " ";
    }
}
