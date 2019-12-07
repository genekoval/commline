#include "generator.h"

#include <commline/commline.h>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

using std::string;

namespace commline::generator {
    /**
     * The default name for the commline configuration file.
     * This file will be read and its contents deserialized.
     */
    const fs::path default_config_path("cli.yaml");

    origin decode_config(fs::path&& path) {
        const auto assert_exists = [&path]() -> fs::file_status {
            auto status = fs::status(path);
            if (status.type() == fs::file_type::not_found)
                throw commline::cli_error("configuration file not found");
            return status;
        };

        if (fs::is_directory(assert_exists())) {
            path /= default_config_path;
            assert_exists();
        }

        return YAML::LoadFile(path).as<origin>();
    }
}

namespace YAML {
    using commline::generator::command;
    using commline::generator::option;
    using commline::generator::origin;

    using std::vector;

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
