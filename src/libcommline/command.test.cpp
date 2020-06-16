#include <test.h>

#include <commline/argv.h>
#include <commline/command.h>

class CommandTest : public testing::Test {
protected:
    static constexpr auto description = "a test command"sv;
    static constexpr auto app_info = commline::app {
        "testapp",
        "0.0.0",
        "Unit tests.",
        "/app"
    };

    std::vector<std::string> arguments;

    auto make_argv(std::initializer_list<std::string> il) -> commline::argv {
        arguments = std::vector<std::string>(il);

        auto argv = commline::argv();
        for (const auto& arg : arguments) argv.push_back(arg);

        return argv;
    }
};

TEST_F(CommandTest, Execution) {
    commline::command(
        "name",
        description,
        commline::options(commline::flag({"help"}, "")),
        [](
            const commline::app& app,
            const commline::argv& argv,
            bool help
        ) {
            ASSERT_EQ(app_info.name, app.name);
            ASSERT_EQ(app_info.version, app.version);
            ASSERT_EQ(app_info.description, app.description);
            ASSERT_EQ(app_info.argv0, app.argv0);

            ASSERT_TRUE(help);
            ASSERT_EQ(1, argv.size());
            ASSERT_EQ("hello"sv, argv[0]);
        }
    )->execute(app_info, make_argv({"--help", "hello"}));
}

TEST_F(CommandTest, Subcommand) {
    constexpr auto argument = "argument";
    constexpr auto cmd = "cmd";

    auto root = commline::command(
        "root",
        description,
        [](
            const commline::app& app,
            const commline::argv& argv
        ) {
            FAIL() << "Command should not run.";
        }
    );

    auto child = commline::command(
        cmd,
        description,
        [argument](
            const commline::app& app,
            const commline::argv& argv
        ) {
            ASSERT_EQ(1, argv.size());
            ASSERT_EQ(argument, argv[0]);
        }
    );

    root->subcommand(std::move(child));

    auto argv = make_argv({cmd, argument});

    auto it = argv.begin();
    auto end = argv.end();

    auto command = root->find(it, end);

    command->execute(app_info, commline::argv(it, end));
}
