#include <iostream>
#include <vector>
#include <algorithm>

std::vector<int> MergeSorted(const std::vector<int>& arr1, const std::vector<int>& arr2)
{
    std::vector<int> result;
    result.reserve(arr1.size() + arr2.size());

    auto arr1_it = arr1.begin();
    auto arr2_it = arr2.begin();
    while (arr1_it != arr1.end() && arr2_it != arr2.end()) {
        if (*arr1_it < *arr2_it) {
            result.push_back(*arr1_it);
            ++arr1_it;
        } else {
            result.push_back(*arr2_it);
            ++arr2_it;
        }
    }

    std::copy(arr1_it, arr1.end(), std::back_inserter(result));
    std::copy(arr2_it, arr2.end(), std::back_inserter(result));
    return result;
}

int main()
{
    std::vector<int> a = {1,2,3,4,5};
    std::vector<int> b = {-1,0};

    std::vector<int> v = MergeSorted(a, b);
    for (const int& i : v) {
        std::cout << i << ", ";
    }
    std::cout << std::endl;
}
