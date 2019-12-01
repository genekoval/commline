#include "generator.h"
#include "template.h"

#include <fstream>
#include <iostream>
#include <cli.h>

using commline::generator::decode_config;

using commline::writer::commands_header;
using commline::writer::fill_template;
using commline::writer::main_source;
using commline::writer::variable;

using std::cout;
using std::ofstream;
using std::string;
using std::vector;

void commline::main(const commline::cli& cli) {
    auto name = cli.options().value("build-name").value();
    auto version = cli.options().value("build-version").value();

    auto origin = decode_config(
        cli.options().value("config-path").value_or(".")
    );

    auto header_path = cli.options().value("header-out").value();

    ofstream header;
    header.open(header_path);
    header << fill_template(commands_header, {
        {variable::commands, [&origin]() {
            return origin.format_command_functions();
        }}
    });
    header.close();

    cout << fill_template(main_source, {
        {variable::commands, [&origin]() { return origin.format_commands(); }},
        {variable::name, [&name]() { return name; }},
        {variable::options, [&origin]() { return origin.format_options(); }},
        {variable::version, [&version]() { return version; }}
    });
}
