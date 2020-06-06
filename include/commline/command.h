#pragma once

#include <commline/argv.h>
#include <commline/context.h>
#include <commline/parameter_list.h>

#include <array>
#include <functional>
#include <memory>
#include <unordered_map>

namespace commline {
    struct command : public describable {
        std::function<void(const app&, const argv&)> execute;
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
            execute = [callable, parameters...](
                const app& context,
                const argv& args
            ) {
                auto arguments = argv();
                auto param_list = parameter_list<Parameters...>(parameters...);

                param_list.parse(
                    args.begin(),
                    args.end(),
                    [&arguments](std::string_view arg) {
                        arguments.push_back(arg);
                    }
                );

                std::apply(callable, std::tuple_cat(
                    std::make_tuple(
                        context,
                        arguments
                    ),
                    param_list.params()
                ));
            };
        }
    };

    class command_node {
        command cmd;
        std::unordered_map<std::string, std::unique_ptr<command_node>> commands;
    public:
        command_node(command&& c) : cmd(c) {}

        auto description() -> std::string_view { return cmd.description; }

        template <typename InputIt>
        auto find(InputIt& first, InputIt last) -> command& {
            auto node = commands.find(std::string(*first));
            if (node != commands.end()) {
                return node->second->find(++first, last);
            }
            else return cmd;
        }

        auto name() -> std::string_view { return cmd.name; }

        auto subcommand(command&& c) -> command_node& {
            auto ret = commands.insert({
                c.name,
                std::make_unique<command_node>(std::move(c))
            });

            return *(ret.first->second);
        }
    };
}
