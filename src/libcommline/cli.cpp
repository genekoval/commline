#include <commline/commline.h>
#include <regex> // regex regex_search

using std::regex;
using std::smatch;
using std::string;
using std::string_view;
using std::vector;

namespace commline {
    static const regex long_opt("^--([^=]+)(?:=(.*))?$");
    static const regex short_opt("^-(.+)$");

    void excess_value(const string& opt) {
        throw cli_error("values not accepted for option: " + opt);
    }

    void missing_value(const string& opt) {
        throw cli_error("missing value for: " + opt);
    }

    cli::cli(string_view exec_path, option_table* option_ptr) :
        m_exec_path(exec_path),
        option_ptr(option_ptr)
    {}

    const vector<string>& cli::args() const { return m_args; }

    string_view cli::exec_path() const { return m_exec_path; }

    const option_table& cli::options() const { return *option_ptr; }

    void cli::parse(unsigned int argc, char** argv) {
        for (auto i = 0u; i < argc; i++) {
            const string arg(argv[i]);
            smatch match;

            if (std::regex_search(arg, match, long_opt)) {
                string key = match[1];
                string value = match[2];
                option& opt = option_ptr->get(key);

                if (opt.has_arg()) {
                    if (value.empty()) missing_value(key);
                    opt.value(value);
                }
                else if (!value.empty()) excess_value(key);
                else opt.m_selected = true;
            }
            else if (std::regex_search(arg, match, short_opt)) {
                // A sequence of short options.
                // For example, options given as '-abc' would appear as the
                // sequence: 'abc'.
                string sequence(match[1]);

                auto it = sequence.begin();
                auto end = sequence.end();

                while (it != end) {
                    string character(1, *(it++));
                    option& opt = option_ptr->get(character);

                    if (opt.has_arg()) {
                        // The option value is the next arg after the sequence
                        // of short options. If there are more options in the
                        // sequence or there are no more args after the
                        // sequence, then the value is missing.
                        if (it != sequence.end() || i + 1 == argc)
                            missing_value(character);
                        opt.value(argv[++i]);
                    }
                    else opt.m_selected = true;
                }
            }
            else m_args.push_back(arg);
        }
    }
}
