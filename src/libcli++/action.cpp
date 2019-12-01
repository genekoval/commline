#include <cli.h>

#include <regex> // regex regex_search

using std::initializer_list;
using std::regex;
using std::smatch;
using std::string;
using std::string_view;
using std::vector;

static const regex long_opt("^--([^=]+)(?:=(.*))?$");
static const regex short_opt("^-(.+)$");

namespace commline {
    void excess_value(const string& opt) {
        throw cli_error("values not accepted for option: " + opt);
    }

    void missing_value(const string& opt) {
        throw cli_error("missing value for: " + opt);
    }

    action::action(
        initializer_list<option> option_list,
        const exec function
    ) :
        function(function),
        opts(option_list)
    {}

    void action::operator()(cli& c, unsigned int argc, char** argv) {
        function(parse(c, argc, argv));
    }

    cli& action::parse(cli& c, unsigned int argc, char** argv) {
        c.option_ptr = &opts;
        auto& args = c.m_args;

        // Firt arg is the exec path. Start from the second arg (index 1).
        for (auto i = 0u; i < argc; i++) {
            const string arg(argv[i]);
            smatch match;

            if (std::regex_search(arg, match, long_opt)) {
                string key = match[1];
                string value = match[2];
                option& opt = opts.get(key);

                if (opt.has_arg()) {
                    if (value.empty()) missing_value(key);
                    opt.value(value);
                }
                else if (!value.empty()) excess_value(key);
                else opt.m_selected = true;
            }
            else if (std::regex_search(arg, match, short_opt)) {
                string sequence(match[1]);

                auto it = sequence.begin();
                auto end = sequence.end();

                while (it != end) {
                    string c(1, *(it++));
                    option& opt = opts.get(c);

                    if (opt.has_arg()) {
                        if (it != sequence.end() || i + 1 == argc)
                            missing_value(c);
                        opt.value(argv[++i]);
                    }
                    else opt.m_selected = true;
                }
            }
            else args.push_back(arg);
        }

        return c;
    }
}
