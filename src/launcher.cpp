#include <nova/launcher.h>
#include <nova/logger.h>

#include <cstdlib>
#include <exception>
#include <iostream>
#include <regex>

using nova::cli::launcher;
using nova::cli::option;
using nova::cli::options;
using std::cout;
using std::endl;
using std::exception;
using std::function;
using std::regex;
using std::regex_search;
using std::smatch;
using std::string;
using std::string_view;
using std::vector;

static bool parse_args(
    options& opts,
    unsigned int argc,
    const char** argv
) {
    for (auto i = 1u; i < argc; i++) {
        const string arg(argv[i]);
        const regex long_re("^--([^=]+)(?:=(.+))?$");
        const regex short_re("^-(.+)$");

        smatch match;

        // Handle long options.
        if (regex_search(arg, match, long_re)) {
            string key = match[1];
            string value = match[2];

            if (!opts.contains(key)) {
                cout << "unrecognized option: " << key << endl;
                return false;
            }

            option& opt = opts.get(key);

            if (opt.has_arg()) {
                if (value.empty()) {
                    cout << "missing value for: " << key << endl;
                    return false;
                }

                opt.value(value);
            } else if (!value.empty()) {
                    cout << "values not required for option: " << key << endl;
                    return false;
                }
        } else if (regex_search(arg, match, short_re)) {
            string seq(match[1]);

            auto it = seq.begin();
            auto end = seq.end();

            while (it != end) {
                string c(1, *(it++));

                if (!opts.contains(c)) {
                    cout << "unrecognized option: " << c << endl;
                    return false;
                }

                option& opt = opts.get(c);

                if (opt.has_arg()) {
                    if (it != seq.end() || i + 1 == argc) {
                        cout << "missing value for: " << c << endl;
                        return false;
                    }

                    opt.value(argv[++i]);
                }
            }
        }
    }

    return true;
}

launcher::launcher(string_view name, string_view version) :
    name(name),
    version(version)
{}

int launcher::start(
    options& opts,
    unsigned int argc,
    const char** argv,
    const function<void(const vector<string>&)>& exec
) {
    if (!parse_args(opts, argc, argv)) return EXIT_FAILURE;

    try {
        exec(args);
    } catch (const exception& ex) {
        cout << "Error: " << ex.what() << endl;
        cout << "program returned " << EXIT_FAILURE << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
