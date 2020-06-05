#pragma once

#include <commline/command.h>

namespace commline {
    class application {
        command_node command_tree;
        const std::string version;
    public:
        template <typename Callable, typename ...Parameters>
        application(
            std::string_view name, 
            std::string_view version, 
            std::string_view description,
            Callable&& callable,
            Parameters&&... parameters
        ) :
            command_tree(command(
                name,
                description,
                std::forward<Callable>(callable),
                std::forward<Parameters>(parameters)...
            )),
            version(version)
        {}

        template <typename InputIt>
        auto run(InputIt first, InputIt last) -> int {
            const auto argv0 = *first;
            auto& cmd = command_tree.find(first, last);
            
            try {
                cmd.execute(
                    app {
                        command_tree.name(),
                        version,
                        command_tree.description(),
                        argv0
                    },
                    commline::argv(first, last)
                );
            } 
            catch (const std::system_error& ex) {
                std::err << ex.what() << std::endl;
                return ex.code().value();
            }
            catch (const std::exception& ex) {
                std::err << ex.what() << std::endl;
                return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        };
    };
}