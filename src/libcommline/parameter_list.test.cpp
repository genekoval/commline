#include <test.h>

class ParameterListTest : public testing::Test {
protected:
    commline::flag flag_opt;
    commline::integer int_opt;
    commline::string string_opt;

    ParameterListTest() :
        flag_opt(
            { "help", "h", "?" },
            "print help message"
        ),
        int_opt(
            {"jobs", "j"},
            "specifies the number of jobs to run simultaneously",
            "jobs",
            1
        ),
        string_opt(
            {"file", "f", "makefile"},
            "Use the specified makefile",
            "file",
            "/home/commline/Makefile"
        )
    {}
};

TEST_F(ParameterListTest, ListCreation) {
    const auto a = "hello"s;
    const auto argv = std::vector<std::string>({"--help", a});
    auto arguments = std::vector<std::string>();

    auto list = commline::parameter_list<commline::flag>(flag_opt);

    list.parse(argv.begin(), argv.end(), [&arguments](const std::string& arg) {
        arguments.push_back(arg);
    });

    ASSERT_EQ(1, arguments.size());
    ASSERT_EQ(a, arguments[0]);

    auto options = list.options();
    auto flag = std::get<0>(options);

    ASSERT_TRUE(flag.value());
}
