//
// Created by steve on 07/01/2025.
//

#include "ParsingHelpers.hpp"

#include <regex>
#include <sstream>

namespace sage::parsing
{

    std::string removeCommentsFromFile(const std::string& fileContents)
    {
        std::stringstream ss(fileContents);
        std::string out;
        out.reserve(fileContents.length()); // Worth it? What if the comments take up a huge amount of space?
        std::string buff;
        while (std::getline(ss, buff, '\n'))
        {
            buff = trim(buff);
            if (buff.starts_with("//"))
            {
                continue;
            }
            auto commentPos = buff.find("//");
            if (commentPos != std::string::npos)
            {
                out.append(buff.substr(0, commentPos) + '\n');
            }
            else
            {
                out.append(buff + '\n');
            }
        }
        return out;
    }

    std::string trim(const std::string& str)
    {
        const auto start = str.find_first_not_of(" \t\n\r");
        const auto end = str.find_last_not_of(" \t\n\r");
        return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
    }

    std::string trimWhiteSpaceFromFile(const std::string& fileContents)
    {
        std::string out;
        out.reserve(fileContents.length());

        std::stringstream ss(fileContents);
        std::string buff;
        ;
        while (std::getline(ss, buff, '\n'))
        {
            out.append(trim(buff) + "\n");
        }
        return out;
    }

    std::string normalizeLineEndings(const std::string& content)
    {
        std::string normalized = content;
        normalized = std::regex_replace(normalized, std::regex("\r\n"), "\n");
        normalized = std::regex_replace(normalized, std::regex("\r"), "\n");
        return normalized;
    }

    TextFunction getFunctionNameAndArgs(const std::string& input)
    {
        std::string trimmedInput = trim(input);

        std::regex pattern(R"(^(\w+)\(([^)]*)\)$)");
        std::smatch match;

        if (std::regex_match(trimmedInput, match, pattern))
        {
            std::string functionName = match[1];
            std::string parameter = match[2];

            return {functionName, parameter};
        }
        else
        {
            return {trimmedInput, ""};
        }
    }
} // namespace sage::parsing
