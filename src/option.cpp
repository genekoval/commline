#include <nova/cli.h>

#include <regex>
#include <utility> // std::move

using nova::cli::cli_error;
using nova::cli::option;
using nova::cli::options;
using std::optional;
using std::regex;
using std::regex_search;
using std::smatch;
using std::string;
using std::string_view;
using std::vector;

static const regex long_re("^--([^=]+)(?:=(.+))?$");
static const regex short_re("^-(.+)$");

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

void nova::cli::parse_cli(
    unsigned int argc,
    const char** argv,
    options& opts,
    vector<string>& args
) {
    for (auto i = 1u; i < argc; i++) {
        const string arg(argv[i]);
        smatch match;

        // Handle long options.
        if (regex_search(arg, match, long_re)) {
            string key = match[1];
            string value = match[2];
            option& opt = opts.get(key);

            if (opt.has_arg()) {
                if (value.empty()) throw cli_error("missing value for: " + key);
                opt.m_value = value;
            } else if (!value.empty())
                throw cli_error("values not required for option: " + key);
        } else if (regex_search(arg, match, short_re)) {
            string seq(match[1]);

            auto it = seq.begin();
            auto end = seq.end();

            while (it != end) {
                string c(1, *(it++));
                option& opt = opts.get(c);

                if (opt.has_arg()) {
                    if (it != seq.end() || i + 1 == argc)
                        throw cli_error("missing value for: " + c);
                    opt.m_value = argv[++i];
                }
            }
        } else args.push_back(arg);
    }
}
