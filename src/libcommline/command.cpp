#include <commline/commline.h>

// TODO Remove this include after initial development.
#include <iostream> // cout endl

using std::initializer_list;
using std::pair;
using std::string;
using std::string_view;
using std::unordered_map;

namespace commline {
    command::command(
        string_view description,
        initializer_list<option> option_list,
        const exec function
    ) :
        m_action(option_list, function),
        m_description(description)
    {}

    const action& command::get_action() { return m_action; }

    origin::origin(
        initializer_list<option> options,
        initializer_list<pair<const string, command>> commands
    ) :
        m_action(options, &main),
        commands(commands)
    {}

    int origin::run(unsigned int argc, char** argv) {
        auto advance_args = [&argc, &argv]() {
            argc--;
            argv++;
        };

        char* exec_path = argv[0];
        advance_args();

        action& act = m_action;

        if (argc > 0) {
            const string possible_command(argv[0]);

            if (commands.find(possible_command) != commands.end()) {
                act = commands.at(possible_command).get_action();
                advance_args();
            }
        }

        try {
            act(exec_path, argc, argv);
            return EXIT_SUCCESS;
        }
        catch (const cli_error& ex) {
            util::print_error(ex);
        }
        catch (const std::exception& ex) {
            util::print_error("fatal error encountered");
            std::cout << ex.what() << std::endl;
            std::cout
                << exec_path
                << ": returned "
                << EXIT_FAILURE
                << std::endl;
        }

        return EXIT_FAILURE;
    }
}
