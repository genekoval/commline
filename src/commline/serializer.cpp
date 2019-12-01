#include "generator.h"

#include <cli.h>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;

using std::string;

namespace commline::generator {
    const string default_config_name("cli.yaml");

    void assert_exists(fs::file_type type) {
        if (type == fs::file_type::not_found)
            throw commline::cli_error("configuration file does not exist");
    }

    origin decode_config(fs::path&& path) {
        auto status = fs::status(path);
        assert_exists(status.type());

        if (fs::is_directory(status)) {
            path /= default_config_name;
            status = fs::status(path);
            assert_exists(status.type());
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
