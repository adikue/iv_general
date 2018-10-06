#include <iostream>
#include <random>

std::random_device r;
std::mt19937 random_engine(r());


void quick_sort(std::vector<int>& array, int start, int end)
{
    if (start >= end) {
        return;
    }

    std::uniform_int_distribution<> uniform_dist(start, end);
    int pos = uniform_dist(random_engine);

    for (int i = start; i <= end; ++i) {
        if ((i < pos && array[i] > array[pos]) ||
            (i > pos && array[i] <= array[pos]))
        {
            std::swap(array[i], array[pos]);
            if (i > pos + 1) {
                std::swap(array[i], array[pos + 1]);
                pos = pos + 1;
            } else {
                pos = i;
            }
        }
    }

    if (pos > start) {
        quick_sort(array, start, pos - 1);
    }
    if (pos < end) {
        quick_sort(array, pos + 1, end);
    }
}





int main()
{
    std::vector<int> v = {1,2,3,4,6,78,8,7,5,4,4,4,5,6,7,4,5,6,3,45,235,7,8,9,98,8};

    quick_sort(v, 0, v.size() - 1);

    for (const auto i : v) {
        std::cout << i << ", ";
    }
    std::cout << std::endl;
}
