#include <iostream>
#include <sstream>

std::string to_decimal(int num, int den)
{
    std::stringstream ss;
    size_t precision = std::numeric_limits<double>::max_digits10 - 1;
    ss.precision(precision);
    ss << std::fixed << (double)num / (double)den;
    std::string str = ss.str();

    // Remove padding
    // This must be done in two steps because of numbers like 700.00
    size_t pos1 = str.find_last_not_of("0");
    if(pos1 != std::string::npos) {
        str.erase(pos1 + 1);
    }
    if (str.back() == '.') {
        str.pop_back();
    }

    size_t point_pos = str.find_first_of(".");
    size_t recurring_start = point_pos;
    size_t decimal_len = str.length() - recurring_start - 1;
    if (decimal_len < precision) {
        return str;
    }

    bool found = false;
    size_t recurring_len = 0;
    while (recurring_start < point_pos + decimal_len / 2) {
        ++recurring_start;
        recurring_len = 1;

        while (recurring_len < decimal_len / 2) {
            std::string recurring_str = str.substr(recurring_start, recurring_len);
            std::string second_str = str.substr(recurring_start + recurring_len, recurring_str.length());

            if (recurring_str == second_str) {
                size_t reminder_start = recurring_start + recurring_len + second_str.length();
                std::string reminder_str = str.substr(reminder_start);
                reminder_str.pop_back();

                size_t check_pos = 0;
                while (check_pos < reminder_str.length()) {
                    std::string rem_substr = reminder_str.substr(check_pos, recurring_str.length());
                    if (recurring_str.compare(0, rem_substr.length(), rem_substr) == 0) {
                        found = true;
                    } else {
                        found = false;
                        break;
                    }
                    check_pos += rem_substr.length();
                }

                break;
            }
            ++recurring_len;
        }
        if (found) {
            break;
        }

    }

    if (found) {
        str.insert(recurring_start, "(");
        str.insert(recurring_start + recurring_len + 1, ")");
        str.erase(recurring_start + recurring_len + 2);

    }

    return str;
}


int main()
{
    std::cout << to_decimal(1, 9) << std::endl;
    std::cout << to_decimal(1, 3) << std::endl;
    std::cout << to_decimal(2, 3) << std::endl;
    std::cout << to_decimal(9, 11) << std::endl;
    std::cout << to_decimal(7, 12) << std::endl;
    std::cout << to_decimal(1, 7) << std::endl;
    std::cout << to_decimal(22, 7) << std::endl;
    std::cout << to_decimal(1, 81) << std::endl;
    std::cout << to_decimal(6666, 10000) << std::endl;
    std::cout << to_decimal(3, 1) << std::endl;
}
