/**
 *  CLOrca is C++ library for fetching and organizing command line arguments
 *  Copyright (C) 2025 Igor Mytsik
 *  Contact me at whitesurfer@protonmail.com
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

#pragma once

#include<iostream>
#include<vector>
#include<cstring>
#include<algorithm>
#include<filesystem>
#include"Option.h"

namespace CLOrca {
    constexpr int unlimited_arguments{-1};

    /**
     * Other configurational variables
     */
    struct Config {
        std::string_view error_prefix{"CLOrca error: "};
        bool verbose{true};
        int arguments_limit{unlimited_arguments};
    };

    /**
     * @see CLOrca::get_option_info()
     */
    struct OptionInfo {
        std::string option;
        std::string value;
        bool has_separator{};
    };

    /**
     * Different types of errors that may occur during the life cycle
     * of an object
     *
     * @see get_error()
     */
    enum Error {
        NoError,
        MissingValue,
        OptionCantHoldValue,
        NotPossibleOption,
        OptionDoesntExist,
        TooMuchArguments,
    };

    class CLOrca {
    protected:
        static constexpr Config default_config{};
        Option* waiting_value_option{};
        std::vector<std::string> arguments;
        const std::vector<std::string> default_arguments;
        std::vector<Option> options;
        const Config config;
        int error{};

        /**
         * Print error
         *
         * @param message Message to print
         */
        void print_error(const std::string& message) const
        {
            if (config.verbose)
                std::cerr << config.error_prefix << message << "\n";
        }

        /**
         * Get option info. Used to obtain data from an argument with combined
         * option and value. e.g. "-u=root", "--file=foo.txt"
         *
         * @param option
         */
        OptionInfo get_option_info(const std::string& option)
        {
            std::string fetched_option, fetched_value;
            bool separator_switch{};

            for (const unsigned char a : option) {
                if (a == SEPARATOR && !separator_switch) {
                    separator_switch = true;
                    continue;
                }

                if (separator_switch)
                    fetched_value += a;
                else
                    fetched_option += a;
            }

            return {fetched_option, fetched_value, separator_switch};
        }

        /**
         * If an option is in short format, then will split all options
         * and process them one by one.
         * e.g. "-laf=foo.txt" = {"-l", "-a", "-f=txt"}
         *
         * @param option
         */
        void load_simple_options(const std::string& option)
        {
            for (int i{}; i < option.size(); ++i) {
                // Skipping "-"
                if (!i)
                    continue;

                if (option.size() > i + 1 && option.at(i + 1) == SEPARATOR) {
                    load_option("-" + option.substr(i));
                    break;
                }
                else
                    load_option("-" + std::string(1, option.at(i)));
            }
        }

        /**
         * Process one option
         *
         * @param option
         */
        void load_option(const std::string& raw_option)
        {
            OptionInfo info{get_option_info(raw_option)};
            Option* cli_option{find_option(info.option)};

            if (!cli_option) {
                error = Error::NotPossibleOption;
                return;
            }

            cli_option->provided = true;

            if (waiting_value_option) {
                error = Error::MissingValue;
                print_error("Got another option while previous option \""
                      + waiting_value_option->aliases.at(0) + "\" is waiting for a value");
                waiting_value_option = nullptr;
            }

            if (cli_option->is_compound()) {
                if (info.has_separator) {
                    if (info.value.size())
                        cli_option->values.push_back(info.value);
                    else {
                        error = Error::MissingValue;
                        print_error("Expecting a value for the option \"" + info.option
                              + "\" after \"" + std::string(1, SEPARATOR) + "\"");
                    }
                } else
                    waiting_value_option = cli_option;
            }
            else if (info.has_separator) {
                error = Error::OptionCantHoldValue;
                print_error("Option \"" + info.option
                            + "\" is not compound and can't hold a value");
            }
        }

        /**
         * Process and load options and arguments
         *
         * @param argc
         * @param argv
         * @return Whether the loading was successful. False if any errors occurred
         */
        bool load_options(const int argc, const char** argv)
        {
            for (int i{}; i < argc; ++i) {
                const std::string curr_arg{argv[i]};

                if (i == 0) {
                    executable_name = std::filesystem::path(curr_arg).filename().string();
                    continue;
                }

                // If current argument is an option
                if (curr_arg.at(0) == '-') {
                    if (curr_arg.size() > 1 && curr_arg.at(1) == '-')
                        load_option(curr_arg);
                    else
                        load_simple_options(curr_arg);
                }
                // If current argument is not an option, then will check
                // if any option is waiting for a value
                else if (waiting_value_option) {
                    waiting_value_option->values.push_back(curr_arg);
                    waiting_value_option = nullptr;
                }
                // If no option is waiting for a value, then just append
                // current argument to arguments
                else
                    arguments.push_back(curr_arg);
            }

            if (waiting_value_option) {
                error = Error::MissingValue;
                print_error("Missing value for option \""
                            + waiting_value_option->aliases.at(0) + "\"");
            }

            if (
                config.arguments_limit != ::CLOrca::unlimited_arguments
                && arguments.size() > config.arguments_limit
            ) {
                error = Error::TooMuchArguments;
                print_error("Unexpected amount of arguments: "
                            + std::to_string(arguments.size())
                            + ". Maximum expected amount is: "
                            + std::to_string(config.arguments_limit)
                );
            }

            return error;
        }

    public:
        static constexpr unsigned char SEPARATOR{'='};
        std::string executable_name;

        /**
         * Constructor
         *
         * @param argc
         * @param argv
         * @param options Possible options
         * @param config Other config variables
         */
        CLOrca(
            const int argc,
            const char** argv,
            const std::vector<Option>& options,
            const std::vector<std::string> default_arguments = {},
            const Config& config = CLOrca::default_config
        ): options(options), config(config), default_arguments(default_arguments)
        {
            load_options(argc, argv);
        }

        /**
         * Find option by its alias
         *
         * @param option Option alias (name)
         * @param verbose Whether to print an error to stderr if option was not found.
         *                If CLOrca::config::verbose is set to false, no message
         *                will be displayed anyways.
         * @return Pointer to an option or nullptr if option wasn't found
         */
        Option* find_option(const std::string& option, const bool verbose = true)
        {
            auto f{std::find_if(options.begin(), options.end(), [option] (Option& o) {
                return o.has_alias(option);
            })};

            if (f != options.end())
                return &(*f);

            if (verbose)
                print_error("Option \"" + option + "\" isn't a possible option");

            return nullptr;
        }

        /**
         * Check if option was provided
         *
         * @param option Option name
         */
        bool check(const std::string& option)
        {
            error = Error::NoError;
            Option* found{find_option(option)};

            if (!found) {
                error = Error::OptionDoesntExist;
                return false;
            }

            return found->provided;
        }

        /**
         * Get value for the requested compound option
         *
         * @param option Option name
         * @param index
         * @see Option::Type
         */
        std::string get(const std::string& option, const int index = 0)
        {
            error = Error::NoError;
            Option* found{find_option(option)};

            if (!found) {
                error = Error::OptionDoesntExist;
                return "";
            }

            return found->get(index);
        }

        /**
         * Get an auto-generated help page. To do so it uses Option's description, name
         * and aliases. So don't leave them blank if you intend to use this function.
         *
         * @param possible_args a list of all possible arguments.
         *                      e.g. "ls /etc /usr" - in this case "/etc" and "/usr"
         *                      are main arguments.
         * @see Option
         */
        std::string get_help(const std::vector<std::string>& possible_args)
        {
            std::string result{"Usage:\n\t" + executable_name};
            std::string usage, str_options;

            for (const Option& o : options) {
                usage += " [" + o.aliases.at(0);

                if (o.is_compound() && o.name.size())
                    usage += "[=]" + o.name;

                usage += "]";
                str_options += "\t" + o.get_aliases() + "\n\t\t" + o.description + "\n";
            }

            for (const std::string& arg : possible_args) {
                usage += " [" + arg + "]";
            }

            str_options = "\n\nOptions:\n" + str_options;

            return result + usage + str_options;
        }

        /**
         * Same as get_help(possible_args) but with a single argument
         *
         * @param possible_arg
         * @see get_help()
         */
        std::string get_help(const std::string& possible_arg = "")
        {
            std::vector<std::string> possible_args{possible_arg};
            return get_help(possible_args);
        }

        /**
         * Get an argument
         *
         * @param argument_number
         */
        std::string get_argument(const int argument_number = 0)
        {
            if (argument_number < arguments.size())
                return arguments.at(argument_number);
            if (argument_number < default_arguments.size())
                return default_arguments.at(argument_number);

            return "";
        }

        /**
         * Get all arguments
         */
        const std::vector<std::string>& get_arguments() const
        {
            return arguments;
        }

        /**
         * Get the most recent error
         */
        int get_error()
        {
            return error;
        }
    };
};
