#include <iostream>
#include <unordered_map>


std::string len_longest(const std::string& s, int k)
{
    std::unordered_map<char, size_t> map;

    if (s.length() < k) {
        return s;
    }

    size_t start = 0, pos = 0;
    std::string res;
    for (; pos < s.length(); ++pos)
    {
        char c = s[pos];
        map[c]++;

        if (map.size() > k) {
            if (pos - start >= res.length()) {
                res = s.substr(start, pos - start);
            }

            while(map.size() > k) {
                char sc = s[start];
                map[sc]--;
                if (map[sc] == 0) {
                    map.erase(sc);
                }
                ++start;
            }
        }
    }

    if (map.size() == 2 && (pos - start) > res.length()) {
        res = s.substr(start, pos - start);
    }
    return res;
}

int main()
{
    // std::string s = "aaaaabbbb";
    std::string s = "abcbbbbcccbdddadacb";
    std::cout << len_longest(s, 3) << std::endl;
}
