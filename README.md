
![CLOrca](https://github.com/user-attachments/assets/3f848ce9-607d-478d-8a20-f7722e091c42)

# CLOrca
CLOrca (abbr. Command Line Orca) - is a compact C++ library for fetching and organizing command line arguments with embedded help page generator.

# Overview
With CLOrca you can:
* specify defaults for compound options and arguments
* set maximum amount of arguments
* specify any amount of aliases for each option
* use compound options with both space ```--name Sam``` and equal sign ```--name=Sam```
* get an auto-generated help page

There are 3 different types of arguments that CLOrca recognizes:
* **Arguments** - arguments that don't belong to any option; e.g. ```ls /usr``` in this case */usr* is an argument
* **Simple options** - options that don't require variable; e.g. ```--help```
* **Compound options** - options that hold value; e.g. ```-f filename.txt``` or ```-n=1```

# Quick start
### Initialization
```cpp
#include"CLOrca.h"
#include<iostream>
#include<vector>

int main(const int argc, const char** argv)
{
    std::vector<CLOrca::Option> possible_options{
        // Name and description of an option will be used during help page generation
        {{"-h", "--help"}, CLOrca::Option::Type::Simple, "help", "print this help page"},
        // You can pass default value(s) for an option as the last parameter
        {{"-p", "--prefix"}, CLOrca::Option::Type::Compound, "prefix", "prefix to a message", "Orca says: "}
    };

    //                                                    argument's default
    CLOrca::CLOrca options(argc, argv, possible_options, {"hello sea world!"});

    // See {@see CLOrca::Error} for error codes
    if (options.get_error()) {
        std::cerr << "CLOrca encountered an error during initialization: " << options.get_error() << "\n";
        return 1;
    }
}
```

### Check if option was passed
```cpp
    if (options.check("-h")) {
        // Option "-h" was passed, the appropriate logic goes here
    }
```

### Get value for an option and get argument
```cpp
    std::cout << options.get("-p") << options.get_argument() << "\n";

    // By default get_argument() returns the first argument that was passed.
    // You can specify an argument number to get any other argument than first.
    const std::string second_orca{options.get_argument(1)};

    if (second_orca.size()) {
        std::cout << "Second orca says: " << second_orca << "\n";
    }
```

### Get an auto-generated help page
```cpp
    if (options.check("--help")) {
        std::vector<std::string> arguments{"message", "2nd_message"};
        std::cout << options.get_help(arguments);

        /**
         * Example output:
         *
         * Usage:
         *    orca_speaks [-h] [-p[=]prefix] [message] [2nd_message]
         *
         * Options:
         *    -h, --help
         *        print this help page
         *    -p, --prefix
         *        prefix to a message
         */

        return 0;
    }
```

### More customization with config variable
```cpp
    CLOrca::Config config{};

    // Do not display any debug messages
    config.verbose = false;

    // Change prefix of all CLOrca debug messages
    config.prefix = "OptionsDebug: ";

    // You can set a maximum amount of arguments that are allowed to be passed
    // to a program. If an argument count exceeds this number, then CLOrca::error
    // will be set during initialization.
    config.arguemnts_limit = 3;

    CLOrca::CLOrca options(argc, argv, possible_options, {"hello sea world!"}, config);
```
You can check the source code of this example [here](examples/orca_says.cpp).
# Credits
This program is written by [Igor Mytsik](mailto:whitesurfer@protonmail.com) and licensed under GNU GPLv3.

This project uses [Catch2](https://github.com/catchorg/Catch2) unit testing library which comes with [BSL-1.0 license](https://github.com/catchorg/Catch2?tab=BSL-1.0-1-ov-file).

CLOrca's logo was made with **Husky Stash** font by [Typodermic Fonts](https://typodermicfonts.com/) which is licensed under Creative Commons Zero.
