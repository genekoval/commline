#pragma once

#include <commline/argv.h>
#include <commline/parameter_list.h>

#include <functional>
#include <unordered_map>

namespace commline {
    class command : public describable {
        std::function<void(const argv&)> exec;
    public:
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
    };
}
