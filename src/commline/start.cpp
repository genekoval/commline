#include "generator.h"
#include "templates.h"

#include <fstream>
#include <iostream>
#include <cli.h>

using commline::generator::decode_config;

using commline::templates::commands_header;
using commline::templates::fill_template;
using commline::templates::main_source;
using commline::templates::variable;

using std::cout;
using std::ofstream;
using std::string;
using std::vector;

namespace fs = std::filesystem;

const fs::path default_commands_header("commline/commands.h");

void open_commands_header(fs::path&& path, ofstream& stream) {
    path /= default_commands_header;
    fs::create_directories(path.parent_path());
    stream.open(path);
}

void commline::main(const commline::cli& cli) {
    auto name = cli.options().value("build-name").value();
    auto version = cli.options().value("build-version").value();

    auto origin = decode_config(
        cli.options().value("config-path").value_or(".")
    );

    ofstream header;
    open_commands_header(
        cli.options().value("header-dir").value_or("."),
        header
    );

    header << fill_template(commands_header, {
        {variable::commands, [&origin]() {
            return origin.format_command_functions();
        }}
    });

    cout << fill_template(main_source, {
        {variable::commands, [&origin]() { return origin.format_commands(); }},
        {variable::name, [&name]() { return name; }},
        {variable::options, [&origin]() { return origin.format_options(); }},
        {variable::version, [&version]() { return version; }}
    });
}
