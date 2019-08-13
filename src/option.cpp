#include <nova/launcher.h>

using nova::cli::option;
using nova::cli::options;
using std::optional;
using std::string;
using std::string_view;

option::option(string_view opt, string_view description) :
    m_description(description),
    m_has_arg(false),
    m_opt(opt)
{}

option::option(string_view opt, bool has_arg, string_view description) :
    m_description(description),
    m_has_arg(has_arg),
    m_opt(opt)
{}

option::option(string_view opt,
    string_view long_opt,
    bool has_arg,
    string_view description
) :
    m_description(description),
    m_has_arg(has_arg),
    m_long_opt(long_opt),
    m_opt(opt)
{}

string option::description() { return m_description; }

bool option::has_arg() { return m_has_arg; }

string option::long_opt() { return m_long_opt; }

string option::opt() { return m_opt; }

string option::value() { return m_value; }

void option::value(string_view val) { m_value = val; }

options& options::add(option opt) {
    opt_map.insert({opt.opt(), opt});
    if (!opt.long_opt().empty()) opt_map.insert({opt.long_opt(), opt});

    return *this;
}

bool options::contains(const string& opt) {
    return opt_map.count(opt) > 0;
}

option& options::get(const string& opt) {
    return opt_map.at(opt);
}

optional<string> options::value(const string& opt) {
    if (!opt_map.count(opt)) return {};
    auto selected = opt_map.at(opt);

    if (selected.value().empty()) return {};
    return selected.value();
}
