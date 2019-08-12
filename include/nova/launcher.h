#pragma once

#include <functional>
#include <string>
#include <string_view>

namespace nova {

    class launcher {
        std::string name;
        std::string version;
    public:
        launcher(std::string_view name, std::string_view version);

        int start(const std::function<void()>& exec);
    };
}
