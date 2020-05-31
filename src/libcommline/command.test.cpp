#include <test.h>

#include <commline/argv.h>
#include <commline/command.h>

constexpr int argc = 2;
const char* argv[argc] = {
    "--help",
    "hello"
};

class CommandTest : public testing::Test {
protected:
    const commline::argv args;

    CommandTest() : args(commline::collect(argc, argv)) {}
};

TEST_F(CommandTest, Creation) {
    commline::command(
        "name",
        "desc",
        [](
            const commline::flag& help,
            const commline::argv& $
        ) {
            ASSERT_TRUE(help.get());
            ASSERT_EQ(1, $.size());
            ASSERT_EQ("hello"s, $[0]);
        },
        commline::flag({"help"}, "")
    )(args);
}
