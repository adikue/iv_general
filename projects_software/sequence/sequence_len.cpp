#include <iostream>
#include <vector>
#include <unordered_set>

int sequence_len(std::vector<int>& v)
{
    std::unordered_set<int> set{v.begin(), v.end()};

    int max = 1;
    for (const int& i : v) {
        int this_len = 1;

        int next = i + 1;
        while (set.count(next)) {
            set.erase(next);
            ++this_len;
            ++next;
        }
        int prev = i - 1;
        while (set.count(prev)) {
            set.erase(prev);
            ++this_len;
            --prev;
        }

        max = std::max(max, this_len);
    }

    return max;
}

int main()
{
    std::vector<int> v = {1,2,3,10,11,12,13,8,9,100,102,103};
    std::cout << sequence_len(v) << std::endl;
}
