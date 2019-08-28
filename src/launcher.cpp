#include <nova/cli.h>
#include <nova/color.h>

#include <array>
#include <cstdlib>
#include <exception>
#include <iostream>

using nova::cli::cli_error;
using nova::cli::exec;
using nova::cli::launcher;
using nova::cli::option;
using nova::cli::option_table;
using nova::cli::parse_cli;
using std::array;
using std::cout;
using std::endl;
using std::exception;
using std::function;
using std::initializer_list;
using std::optional;
using std::runtime_error;
using std::string;
using std::string_view;
using std::vector;

static const array<option,1> default_options {
    option("v", "version", false, "print the version number and exit")
};

cli_error::cli_error(const string& message) : runtime_error(message) {}

launcher::launcher(
    string_view name,
    string_view version,
    initializer_list<option> option_list
) :
    name(name),
    opts(option_list),
    version(version)
{}

const vector<string>& launcher::args() const { return m_args; }

const option_table& launcher::options() const { return opts; }

void launcher::print_error(string_view text) const {
    using namespace nova::termcolor;

    cout
        << set(format::bold, color::red)
        << "error: "
        << reset()
        << text
        << endl;
}

void launcher::print_version() const {
    cout << name << " " << version << endl;
}

int launcher::start(unsigned int argc, const char** argv, const exec& program) {
    for (auto& opt : default_options)
        opts.add(opt);

    try {
        parse_cli(argc, argv, opts, m_args);

        if (opts.selected("version")) print_version();
        else program(*this);

        return EXIT_SUCCESS;
    } catch (const cli_error& ex) {
        print_error(ex.what());
    } catch (const exception& ex) {
        cout << "Error: " << ex.what() << endl;
        cout << "program returned " << EXIT_FAILURE << endl;
    }

    return EXIT_FAILURE;
}
