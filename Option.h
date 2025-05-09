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

#pragma once

#include<vector>
#include<string>
#include<algorithm>

namespace CLOrca {
    class Option {
    public:
        /**
         * Option types
         */
        enum class Type {
            Simple,
            Compound
        };

        const std::vector<std::string> aliases;
        const std::string description;
        const std::string name;
        std::vector<std::string> values;

        /**
         * @var Default option values. Be aware that even though you can pass
         *      default values for a simple option, they won't be used for
         *      anything. The reason for that is that a simple option's role is to
         *      just answer whether the option was specified or not. For any other
         *      purpose you should use compound options.
         */
        std::vector<std::string> defaults;
        const Type type;

        /** @var Whether an option was provided by user */
        bool provided{};

        /**
         * Constructor
         *
         * @param aliases All the aliases option uses. e.g. {"-h", "--help"}
         * @param type Type of the option. Compound options can hold a value e.g. "-f file.txt"
         *                                 Simple options can't hold a value e.g. "-h"
         * @param name Option name. Used in generation of auto-help. {@see CLOrca::get_help()}
         * @param description Description. Used in generation of auto-help.
         * @param defaults Default values for an option
         */
       Option(
            const std::vector<std::string> aliases,
            const Type type,
            const std::string& name = "",
            const std::string& description = "",
            const std::vector<std::string>& defaults = {}
        ): type(type), aliases(aliases), description(description), name(name), defaults(defaults)
        {
        }

        /**
         * Overridden in case user passes just one default option
         *
         * @see Option()
         */
        template<typename T>
        Option(
            const std::vector<std::string> aliases,
            const Type type,
            const std::string& name,
            const std::string& description,
            const T& defaults
        ): type(type), aliases(aliases), description(description), name(name), defaults({defaults})
        {
        }

        /**
         * Whether an option is identified by specified alias
         *
         * @param alias Alias name
         */
        bool has_alias(const std::string& alias) const
        {
            auto f{std::find_if(aliases.begin(), aliases.end(), [&alias] (const std::string& a) {
                return alias == a;
            })};

            return f != aliases.end();
        }

        /**
         * Whether an option is compound
         */
        bool is_compound() const
        {
            return type == Type::Compound;
        }

        /**
         * Get value for a compound option
         *
         * @param index Value index. e.g. if "./foo -f bar.txt -f test.txt", then
         *              get() will return bar.txt, get(1) will return test.txt
         */
        std::string get(const int index = 0) const
        {
            if (!is_compound())
                return "";

            if (values.size() > index)
                return values.at(index);
            if (defaults.size() > index)
                return defaults.at(index);

            return "";
        }

        /**
         * Get all aliases as a string separated by {@param unifying_str}
         *
         * @param unifying_str
         */
        std::string get_aliases(const std::string& unifying_str = ", ") const
        {
            std::string result;

            for (const std::string& alias : aliases) {
                if (result.size()) {
                    result += unifying_str;
                }

                result += alias;
            }

            return result;
        }
    };
};
