#include <test.h>

#include <commline/argv.h>
#include <commline/command.h>

class CommandTest : public testing::Test {
protected:
    static constexpr auto description = "a test command"sv;

    std::vector<std::string> arguments;

    auto make_argv(std::initializer_list<std::string> il) -> commline::argv {
        arguments = std::vector<std::string>(il);

        auto argv = commline::argv();
        for (const auto& arg : arguments) argv.push_back(arg);

        return argv;
    }
};

TEST_F(CommandTest, Creation) {
    commline::command(
        "name",
        description,
        [](
            const commline::flag& help,
            const commline::argv& argv
        ) {
            ASSERT_TRUE(help.get());
            ASSERT_EQ(1, argv.size());
            ASSERT_EQ("hello"sv, argv[0]);
        },
        commline::flag({"help"}, "")
    ).execute(make_argv({"--help", "hello"}));
}

TEST_F(CommandTest, Subcommand) {
    constexpr auto argument = "argument";
    constexpr auto cmd = "cmd";

    auto parent = commline::command(
        "root",
        description,
        [](const commline::argv& argv) {
            FAIL() << "Command should not run.";
        }
    );

    auto child = commline::command(
        cmd,
        description,
        [argument](const commline::argv& argv) {
            ASSERT_EQ(1, argv.size());
            ASSERT_EQ(argument, argv[0]);
        }
    );

    auto root = commline::command_node(std::move(parent));
    root.subcommand(std::move(child));

    auto argv = make_argv({cmd, argument});

    auto it = argv.begin();
    auto end = argv.end();

    auto& command = root.find(it, end);

    command.execute(commline::argv(it, end));
}
