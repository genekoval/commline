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
    auto list = commline::parameter_list<commline::flag>(flag_opt);
}
