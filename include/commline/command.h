#pragma once

#include <commline/argv.h>
#include <commline/parameter_list.h>

#include <functional>
#include <unordered_map>

namespace commline {
    class command_handler : public describable {
        std::unordered_map<std::string_view, const command_handler*> commands;
        std::function<void(const argv&)> exec;
    public:
        const std::string name;

        template <typename Callable, typename ...Parameters>
        command_handler(
            std::string_view name, 
            std::string_view description,
            Callable&& callable,
            Parameters&&... parameters

        ) :
            describable(description),
            name(name)
        {
            exec = [callable, parameters...](const argv& args) {
                auto arguments = argv();
                auto param_list = parameter_list<Parameters...>(parameters...);

                param_list.parse(
                    args.begin(),
                    args.end(),
                    [&arguments](std::string_view arg) {
                        arguments.push_back(arg);
                    }
                );

                std::apply(
                    callable,
                    std::tuple_cat(
                        param_list.params(),
                        std::make_tuple(arguments)
                    )
                );
            };
        }

        auto operator()(const argv& args) -> void { exec(args); }

        auto subcommand(const command_handler& c) -> void {
            commands[c.name] = &c;
        }
    };

    class command_repo {
        std::vector<command_handler> storage;
    public:
        auto command(command_handler&& c) -> const command_handler& {
            storage.push_back(std::move(c));
            return storage.back();
        }
    };
}
