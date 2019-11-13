#include <functional>
#include <iomanip>
#include <iostream>
#include <nova/cli.h>
#include <nova/ext/string.h>
#include <regex>
#include <resources.h>
#include <sstream>
#include <string_view>
#include <unordered_map>
#include <yaml-cpp/yaml.h>

using namespace std::literals;

using nova::cli::launcher;
using nova::cli::option;

constexpr auto variable_regex = "(\\$[a-zA-Z0-9]+)"sv;

std::string get_source_template() {
    return std::string(
        reinterpret_cast<char*>(resources_main_template_cpp),
        resources_main_template_cpp_len
    );
}

void nova::cli::start(const launcher& cli) {
    // Returns the value assigned to the specified option as a quoted string.
    const auto make_string =
        [&cli](const std::string& option) -> std::string {
            if (!cli.options().selected(option))
                throw nova::cli::cli_error("missing option: " + option);

            std::ostringstream os;
            os << cli.options().value(option).value();
            return os.str();
        };

    std::unordered_map<
        std::string,
        std::function<std::string()>
    > variable_handlers;

    variable_handlers["name"] = [&make_string]() {
        return make_string("build-name");
    };

    variable_handlers["options"] = [&cli]() {
        if (!cli.options().selected("config-path"))
            throw nova::cli::cli_error("missing option: config-path");

        auto config = YAML::LoadFile(cli.options().value("config-path").value());

        if (!config["options"])
            throw nova::cli::cli_error("'options' node missing in configuration file");

        auto options = config["options"];
        if (!options.IsSequence())
            throw nova::cli::cli_error("'options' node must be a sequence");

        std::ostringstream os;
        os.fill('\t');

        auto indent = [&os](int spaces) {
            os << std::endl << std::setw(spaces) << "";
        };

        auto it = options.begin();
        auto end = options.end();

        while (it != end) {
            auto option = *it;

            os << "option(";

            indent(3);
            os << std::quoted(option["flag"].as<std::string>()) << ",";

            indent(3);
            os << std::quoted(option["long"].as<std::string>()) << ",";

            indent(3);
            if (option["type"]) os << "true";
            else os << "false";
            os << ",";

            indent(3);
            os << std::quoted(option["description"].as<std::string>());

            indent(2);
            os << ")";

            if (++it != end) {
                os << ",";
                indent(2);
            }
        }

        return os.str();

    };

    variable_handlers["version"] = [&make_string]() {
        return make_string("build-version");
    };

    std::cout << nova::ext::string::replace<std::string>(
        get_source_template(),
        std::regex(std::string(variable_regex)),
        [&variable_handlers](const std::smatch& match) {
            std::string variable = std::string(match[1]).substr(1);
            return variable_handlers.at(variable)();
        }
    );
}

int main(int argc, const char** argv) {
    return launcher(NAME, VERSION, {
        option(
            "n",
            "build-name",
            true,
            "Program name"
        ),
        option(
            "c",
            "config-path",
            true,
            "Path to the configuration file"
        ),
        option(
            "V",
            "build-version",
            true,
            "Program version"
        )
    }).start(argc, argv);
}
