#include "generator.h"
#include "template.h"

#include <fstream>
#include <iostream>
#include <cli.h>
#include <yaml-cpp/yaml.h>

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
    auto config_path = cli.options().value("config-path").value();
    auto header_path = cli.options().value("header-out").value();

    auto origin = YAML::LoadFile(config_path).as<commline::generator::origin>();

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

namespace YAML {
    using commline::generator::command;
    using commline::generator::option;
    using commline::generator::origin;

    template<>
    struct convert<option> {
        static bool decode(const Node& node, option& opt) {
            opt.short_opt = node["short"].as<string>();
            opt.long_opt = node["long"].as<string>();
            opt.has_arg = node["has arg?"].as<bool>();
            opt.description = node["description"].as<string>();

            return true;
        }
    };

    template<>
    struct convert<command> {
        static bool decode(const Node& node, command& com) {
            com.name = node["name"].as<string>();
            com.description = node["description"].as<string>();
            com.options = node["options"].as<vector<option>>();

            return true;
        }
    };

    template<>
    struct convert<origin> {
        static bool decode(const Node& node, origin& ori) {
            auto commands = node["commands"];
            auto options = node["options"];

            if (commands) ori.commands = commands.as<vector<command>>();
            if (options) ori.options = options.as<vector<option>>();

            return true;
        }
    };
}
