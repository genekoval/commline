#pragma once

#include <commline/argv.h>
#include <commline/parameter_list.h>

#include <array>
#include <functional>
#include <memory>
#include <unordered_map>

namespace commline {
    class command : public describable {
        std::unordered_map<std::string, std::shared_ptr<command>> commands;
    public:
        std::function<void(const argv&)> execute;
        const std::string name;

        template <typename Callable, typename ...Parameters>
        command(
            std::string_view name,
            std::string_view description,
            Callable&& callable,
            Parameters&&... parameters

        ) :
            describable(description),
            name(name)
        {
            execute = [callable, parameters...](const argv& args) {
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
    };

    class command_node {
        command cmd;
        std::unordered_map<std::string, std::unique_ptr<command_node>> commands;
    public:
        command_node(command&& c) : cmd(c) {}

        template <typename InputIt>
        auto find(InputIt& first, InputIt last) -> command& {
            auto node = commands.find(std::string(*first));
            if (node != commands.end()) {
                return node->second->find(++first, last);
            }
            else return cmd;
        }

        auto subcommand(command&& c) -> command_node* {
            auto name = std::string(c.name);

            auto ret = commands.insert({
                name,
                std::make_unique<command_node>(std::move(c))
            });

            if (!ret.second) throw cli_error(
                "Failed to register command: " + name
            );

            return ret.first->second.get();
        }
    };
}
