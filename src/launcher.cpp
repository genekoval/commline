#include <nova/cli.h>
#include <nova/logger.h>

#include <cstdlib>
#include <exception>
#include <iostream>
#include <regex>

using nova::cli::cli_error;
using nova::cli::launcher;
using nova::cli::option;
using nova::cli::options;
using std::cout;
using std::endl;
using std::exception;
using std::function;
using std::regex;
using std::regex_search;
using std::runtime_error;
using std::smatch;
using std::string;
using std::string_view;
using std::vector;

cli_error::cli_error(const string& message) : runtime_error(message) {}

static void parse_args(
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
            option& opt = opts.get(key);

            if (opt.has_arg()) {
                if (value.empty()) throw cli_error("missing value for: " + key);
                opt.value(value);
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
                    opt.value(argv[++i]);
                }
            }
        }
    }
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
    try {
        parse_args(opts, argc, argv);
        exec(args);
        return EXIT_SUCCESS;
    } catch (const cli_error& ex) {
        cout << "error: " << ex.what() << endl;
    } catch (const exception& ex) {
        cout << "Error: " << ex.what() << endl;
        cout << "program returned " << EXIT_FAILURE << endl;
    }

    return EXIT_FAILURE;
}
