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

#include"../CLOrca.h"
#include<iostream>
#include<vector>

/**
 * Example echo program
 */
int main(const int argc, const char** argv)
{
    std::vector<CLOrca::Option> possible_options{
        {{"-h", "--help"}, CLOrca::Option::Type::Simple, "help", "print this help page"},
        {{"-p", "--prefix"}, CLOrca::Option::Type::Compound, "prefix", "prefix to a message", "Orca says: "}
    };

    CLOrca::CLOrca options(argc, argv, possible_options, {"hello sea world!"});

    if (options.get_error()) {
        std::cerr << "CLOrca encountered an error during initialization: " << options.get_error() << "\n";
        return 1;
    }

    if (options.check("-h")) {
        std::vector<std::string> arguments{"message", "2nd_message"};
        std::cout << options.get_help(arguments);

        return 0;
    }

    std::cout << options.get("-p") << options.get_argument() << "\n";
    const std::string second_orca{options.get_argument(1)};

    if (second_orca.size()) {
        std::cout << "Second orca says: " << second_orca << "\n";
    }

    return 0;
}
