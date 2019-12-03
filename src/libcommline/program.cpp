#include <cli.h>

using std::string;
using std::string_view;

namespace commline {
    program::program(string_view name, string_view version) :
        m_name(name), m_version(version)
    {}

    string_view program::name() { return m_name; }

    string_view program::version() { return m_version; }
}
