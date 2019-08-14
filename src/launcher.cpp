#include <nova/cli.h>
#include <nova/color.h>

#include <cstdlib>
#include <exception>
#include <iostream>

using nova::cli::cli_error;
using nova::cli::exec;
using nova::cli::launcher;
using nova::cli::option;
using nova::cli::options;
using nova::cli::parse_cli;
using std::cout;
using std::endl;
using std::exception;
using std::function;
using std::runtime_error;
using std::string;
using std::string_view;
using std::vector;

cli_error::cli_error(const string& message) : runtime_error(message) {}

launcher::launcher(string_view name, string_view version, options& opts) :
    name(name),
    opts(opts),
    version(version)
{}

const vector<string>& launcher::args() const { return m_args; }

void launcher::print_error(string_view text) {
    using namespace nova::termcolor;

    cout
        << set(format::bold, color::red)
        << "error: "
        << reset()
        << text
        << endl;
}

int launcher::start(unsigned int argc, const char** argv, const exec& program) {
    try {
        parse_cli(argc, argv, opts, m_args);
        program(*this);

        return EXIT_SUCCESS;
    } catch (const cli_error& ex) {
        print_error(ex.what());
    } catch (const exception& ex) {
        cout << "Error: " << ex.what() << endl;
        cout << "program returned " << EXIT_FAILURE << endl;
    }

    return EXIT_FAILURE;
}
