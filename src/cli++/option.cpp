#include <nova/cli.h>

#include <regex> // regex regex_search

using nova::cli::cli_error;
using nova::cli::option;
using nova::cli::option_table;
using std::initializer_list;
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
    m_opt(opt),
    m_selected(false)
{}

option::option(string_view opt, bool has_arg, string_view description) :
    m_description(description),
    m_has_arg(has_arg),
    m_opt(opt),
    m_selected(false)
{}

option::option(
    string_view opt,
    string_view long_opt,
    bool has_arg,
    string_view description
) :
    m_description(description),
    m_has_arg(has_arg),
    m_long_opt(long_opt),
    m_opt(opt),
    m_selected(false)
{}

string option::description() const { return m_description; }

bool option::has_arg() const { return m_has_arg; }

string option::long_opt() const { return m_long_opt; }

string option::opt() const { return m_opt; }

bool option::selected() const { return m_selected; }

string option::value() const { return m_value; }

void option::value(string_view val) {
    m_value = val;
    m_selected = true;
}

option_table::option_table(initializer_list<option> option_list) :
    opts(option_list)
{
    for (auto& opt : opts) add_to_map(&opt);
}

void option_table::add(const option& opt) {
    opts.push_back(opt);
    add_to_map(&opts.back());
}

void option_table::add_to_map(option* opt) {
    for (auto& key : {opt->opt(), opt->long_opt()})
        if (!key.empty()) opt_map.insert({key, opt});
}

option& option_table::get(const string& key) {
    if (!opt_map.count(key)) throw cli_error("unknown option: " + key);
    return *(opt_map.at(key));
}

bool option_table::selected(const string& key) const {
    if (!opt_map.count(key)) return false;
    return opt_map.at(key)->selected();
}

optional<string> option_table::value(const string& key) const {
    if (!opt_map.count(key)) return {};

    auto* opt = opt_map.at(key);
    if (opt->value().empty()) return {};

    return opt->value();
}

void nova::cli::parse_cli(
    unsigned int argc,
    const char** argv,
    option_table& opts,
    vector<string>& args
) {
    for (auto i = 1u; i < argc; i++) {
        const string arg(argv[i]);
        smatch match;

        if (regex_search(arg, match, long_re)) {
            string key = match[1];
            string value = match[2];
            option& opt = opts.get(key);

            if (opt.has_arg()) {
                if (value.empty()) throw cli_error("missing value for: " + key);
                opt.value(value);
            }
            else if (!value.empty())
                throw cli_error("values not required for option: " + key);
            else opt.m_selected = true;
        }
        else if (regex_search(arg, match, short_re)) {
            string seq(match[1]);

            auto it = seq.begin();
            auto end = seq.end();

            while (it != end) {
                string c(1, *(it++));
                option& opt = opts.get(c);

                if (opt.has_arg()) {
                    if (it != seq.end() || i + 1 == argc)
                        throw cli_error("missing value for: " + c);
                    opt.value(argv[++i]);
                }
                else opt.m_selected = true;
            }
        }
        else args.push_back(arg);
    }
}
