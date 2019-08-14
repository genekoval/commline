#include <nova/cli.h>

#include <utility> // std::move

using nova::cli::cli_error;
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

options& options::add(option&& opt) {
    opts.push_back(std::move(opt));
    option& opt_ref = opts.back();

    opt_map.insert({opt_ref.opt(), &opt_ref});
    if (!opt_ref.long_opt().empty())
        opt_map.insert({opt_ref.long_opt(), &opt_ref});

    return *this;
}

option& options::get(const string& opt) {
    if (!opt_map.count(opt)) throw cli_error("unrecognized option: " + opt);
    return *(opt_map.at(opt));
}

optional<string> options::value(const string& opt) {
    if (!opt_map.count(opt)) return {};
    auto* selected = opt_map.at(opt);

    if (selected->value().empty()) return {};
    return selected->value();
}
