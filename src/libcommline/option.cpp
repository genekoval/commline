#include <commline/commline.h>

#include <regex> // regex regex_search

using std::initializer_list;
using std::optional;
using std::regex;
using std::regex_search;
using std::smatch;
using std::string;
using std::string_view;
using std::vector;

static const regex long_re("^--([^=]+)(?:=(.*))?$");
static const regex short_re("^-(.+)$");

namespace commline {
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
}
