#include <cli.h>

using std::string;
using std::string_view;
using std::vector;

namespace commline {
    cli::cli(string_view exec_path) : m_exec_path(exec_path) {}

    const vector<string>& cli::args() const { return m_args; }

    string_view cli::exec_path() const { return m_exec_path; }

    const option_table& cli::options() const { return *option_ptr; }
}
