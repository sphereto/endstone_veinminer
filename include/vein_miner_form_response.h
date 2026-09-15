#pragma once

#include <cctype>
#include <string>
#include <variant>
#include <vector>

namespace veinminer {

using FormValue = std::variant<bool, double, std::string>;

class FormResponseParser {
public:
    static std::vector<FormValue> Parse(const std::string &raw)
    {
        std::vector<FormValue> values;
        size_t i = 0;
        SkipWhitespace(raw, i);

        if (i >= raw.size() || raw[i] != '[') {
            return values;
        }
        ++i;

        while (i < raw.size()) {
            SkipWhitespace(raw, i);
            if (i < raw.size() && raw[i] == ']') {
                break;
            }
            values.push_back(ParseValue(raw, i));
            SkipWhitespace(raw, i);
            if (i < raw.size() && raw[i] == ',') {
                ++i;
            }
        }

        return values;
    }

private:
    static void SkipWhitespace(const std::string &raw, size_t &i)
    {
        while (i < raw.size() && std::isspace(static_cast<unsigned char>(raw[i]))) {
            ++i;
        }
    }

    static FormValue ParseValue(const std::string &raw, size_t &i)
    {
        SkipWhitespace(raw, i);

        if (raw.compare(i, 4, "true") == 0) {
            i += 4;
            return true;
        }
        if (raw.compare(i, 5, "false") == 0) {
            i += 5;
            return false;
        }
        if (i < raw.size() && raw[i] == '"') {
            return ParseString(raw, i);
        }
        return ParseNumber(raw, i);
    }

    static std::string ParseString(const std::string &raw, size_t &i)
    {
        ++i;
        std::string out;
        while (i < raw.size() && raw[i] != '"') {
            if (raw[i] == '\\' && i + 1 < raw.size()) {
                ++i;
            }
            out.push_back(raw[i]);
            ++i;
        }
        if (i < raw.size()) {
            ++i;
        }
        return out;
    }

    static double ParseNumber(const std::string &raw, size_t &i)
    {
        const size_t start = i;
        while (i < raw.size() && (std::isdigit(static_cast<unsigned char>(raw[i])) || raw[i] == '-' ||
                                  raw[i] == '+' || raw[i] == '.' || raw[i] == 'e' || raw[i] == 'E')) {
            ++i;
        }
        if (i == start) {
            ++i;
            return 0.0;
        }
        return std::stod(raw.substr(start, i - start));
    }
};

}  // namespace veinminer
