#include <test.h>

#include <commline/argv.h>
#include <commline/command.h>

class CommandTest : public testing::Test {
protected:
    static constexpr auto description = "desc"sv;

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
    )(make_argv({"--help", "hello"}));
}

TEST_F(CommandTest, Subcommand) {
    auto commands = commline::command(
        "first",
        description,
        [](
            const commline::argv& argv
        ) {
            FAIL() << "Command should not run.";
        }
    ).subcommand(
        "second",
        description,
        [](
            const commline::flag opt,
            const commline::argv& argv
        ) {
            ASSERT_TRUE(opt.get());
            ASSERT_EQ(1, argv.size());
            ASSERT_EQ("arg", argv[0]);
        },
        commline::flag({"opt"}, "")
    );

    auto argv = make_argv({
        "second", "--opt", "arg"
    });

    commands.run(argv.begin(), argv.end());
}
