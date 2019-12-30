#include <generator.h>
#include <templates.h>

#include <commline/commline.h>
#include <fstream>
#include <iostream>

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

const fs::path header_dir(commline::current_program().name());

const fs::path program_header_file(header_dir / (header_dir.string() + ".h"));
const fs::path commands_header_file(header_dir / fs::path("commands.h"));

void write_commands_header(
    fs::path&& path,
    const commline::generator::origin& origin
) {
    path /= commands_header_file;
    fs::create_directories(path.parent_path());

    ofstream header;
    header.open(path);

    header << fill_template(commands_header, {
        {variable::commands, [&origin]() {
            return origin.format_command_functions();
        }}
    }) << std::flush;
}

void commline::main(const commline::cli& cli) {
    auto name = cli.options().value("build-name").value();
    auto version = cli.options().value("build-version").value();

    auto origin = decode_config(
        cli.options().value("config-path").value_or(".")
    );

    auto* header = &program_header_file;

    if (!origin.commands.empty()) {
        header = &commands_header_file;
        write_commands_header(
            cli.options().value("header-dir").value_or("."), origin
        );
    }

    cout << fill_template(main_source, {
        {variable::commands, [&origin]() { return origin.format_commands(); }},
        {variable::header, [header]() { return *header; }},
        {variable::name, [&name]() { return name; }},
        {variable::options, [&origin]() { return origin.format_options(); }},
        {variable::version, [&version]() { return version; }}
    });
}
