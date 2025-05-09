/**
 *  This file is part of CLOrca.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include<catch2/catch_amalgamated.hpp>
#include"../CLOrca.h"

const char* argv[]{
    "tests",
    "-f",
    "filename.txt",
    "-h",
    "argument1",
    "argument2",
    "-la=foo.txt",
    "-f=filename2.txt",
    "-d",
    "default_option1",
    "--default=default_option2"
};
const int argc{11};

std::vector<CLOrca::Option> input_options{
    {{"-h", "--help"}, CLOrca::Option::Type::Simple, "help", "print help page"},
    {{"-f", "--file"}, CLOrca::Option::Type::Compound, "file", "name of the file"},
    {{"-l"}, CLOrca::Option::Type::Simple, "list", "list all the possible outcomes"},
    {{"-a"}, CLOrca::Option::Type::Compound, "append", "append provided line to the file"},
    {{"-d", "--default"}, CLOrca::Option::Type::Compound, "default", "default options", {"1", "2", "default_option3"}},
};

TEST_CASE("Main test case", "[main]") {
    CLOrca::CLOrca options{argc, argv, input_options, {"default_arg1", "default_arg2", "default_arg3"}};

    REQUIRE_FALSE(options.get_error());
    CHECK(options.check("-h"));
    CHECK(options.check("--help"));
    CHECK(options.check("-f"));
    CHECK(options.check("--file"));
    CHECK(options.get("-f") == "filename.txt");
    CHECK(options.get("-f", 1) == "filename2.txt");
    CHECK(options.get_arguments().size() == 2);
    CHECK(options.get_argument() == "argument1");
    CHECK(options.get_argument(1) == "argument2");
    CHECK(options.check("-l"));
    CHECK(options.check("-a"));
    CHECK(options.get("-a") == "foo.txt");
    CHECK(options.check("-d"));
    CHECK(options.check("--default"));
    CHECK(options.get("-d") == "default_option1");
    CHECK(options.get("--default", 1) == "default_option2");
    CHECK(options.get("-d", 2) == "default_option3");
    CHECK(options.get("--default", 2) == "default_option3");
    CHECK(options.get_argument(2) == "default_arg3");
}

TEST_CASE("Testing arguments limit", "[arguments_limit]") {
    const char* argv1[]{
        "tests",
        "-h",
        "argument",
        "argument2"
    };
    const int argc1{3};

    std::vector<CLOrca::Option> input_options{
        {{"-h", "--help"}, CLOrca::Option::Type::Simple, "help", "print help page"},
    };

    CLOrca::Config config;
    config.arguments_limit = 1;
    config.verbose = false;
    CLOrca::CLOrca options{argc1, argv1, input_options, {}, config};

    REQUIRE_FALSE(options.get_error());
    CHECK(options.check("-h"));
    CHECK(options.check("--help"));
    CHECK(options.get_argument() == "argument");

    CLOrca::CLOrca options_two{argc1 + 1, argv1, input_options, {}, config};
    REQUIRE(options_two.get_error() == CLOrca::Error::TooMuchArguments);
}

TEST_CASE("Testing general error cases", "[errors]") {
    const char* argv1[]{
        "tests",
        "-f",
    };
    CLOrca::CLOrca options{2, argv1, {
        {{"-f", "--file"}, CLOrca::Option::Type::Compound, {}, "file", "name of the file"},
        {{"-h", "--help"}, CLOrca::Option::Type::Simple, {}, "help", "print help page"},
    }, {}, {"", false}};

    CHECK(options.get_error() == CLOrca::Error::MissingValue);

    const char* argv2[]{
        "tests",
        "-h=\"test\""
    };
    CLOrca::CLOrca options_two{2, argv2, {
        {{"-h", "--help"}, CLOrca::Option::Type::Simple, {}, "help", "print help page"},
    }, {}, {"", false}};

    CHECK(options_two.get_error() == CLOrca::Error::OptionCantHoldValue);

    options_two.get("--non-existent");
    CHECK(options_two.get_error() == CLOrca::Error::OptionDoesntExist);

    options_two.get("-h");
    CHECK(options_two.get_error() == CLOrca::Error::NoError);

    options_two.check("-h");
    CHECK(options_two.get_error() == CLOrca::Error::NoError);

    options_two.check("--non-existent");
    CHECK(options_two.get_error() == CLOrca::Error::OptionDoesntExist);

    options_two.check("-h");
    CHECK(options_two.get_error() == CLOrca::Error::NoError);

    const char* argv3[]{
        "tests",
        "--doesnt-exist",
    };

    CLOrca::CLOrca options_three{2, argv3, {
        {{"-f", "--file"}, CLOrca::Option::Type::Compound, {}, "file", "name of the file"},
        {{"-h", "--help"}, CLOrca::Option::Type::Simple, {}, "help", "print help page"},
    }, {}, {"", false}};

    CHECK(options_three.get_error() == CLOrca::Error::NotPossibleOption);
}

TEST_CASE("Benchmark", "[!benchmark]") {
    BENCHMARK("CLOrca object initialization benchmark") {
        CLOrca::CLOrca options_bench{argc, argv, input_options};
    };
}
