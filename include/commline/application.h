#pragma once

#include <commline/command.h>

#include <system_error>
#include <iostream>

namespace commline {
    template <typename Callable, typename ...Options>
    class application : public command_impl<Callable, Options...> {
    public:
        const std::string version;

        application(
            std::string_view name,
            std::string_view version,
            std::string_view description,
            Callable fn
        ) :
            command_impl<Callable, Options...>(name, description, fn),
            version(version)
        {}

        application(
            std::string_view name,
            std::string_view version,
            std::string_view description,
            std::tuple<Options...>&& opts,
            Callable fn
        ) :
            command_impl<Callable, Options...>(name, description, opts, fn),
            version(version)
        {}

        template <typename InputIt>
        auto run(InputIt first, InputIt last) -> int {
            const auto argv0 = *(first++);
            auto cmd = this->find(first, last);

            try {
                cmd->execute({
                        this->name,
                        version,
                        this->description,
                        argv0
                    },
                    argv(first, last)
                );
            }
            catch (const std::system_error& ex) {
                std::cerr << ex.what() << std::endl;
                return ex.code().value();
            }
            catch (const std::exception& ex) {
                std::cerr << ex.what() << std::endl;
                return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        };
    };
}
