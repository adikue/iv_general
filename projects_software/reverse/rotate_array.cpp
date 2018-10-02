#include <iostream>
#include <vector>


void rotate_array(std::vector<int>& data, int k)
{
    size_t length = data.size();

    if (k < 0) {
        throw std::runtime_error("illegal steps value");
    }
    if (k == 0 || length == 0) {
        return;
    }
    if (k > length) {
        k = k % length;
    }

    // reverse whole vector O(n/2)
    for (int i = 0; i < length/2; ++i)
    {
        if (data[i] == data[length - i - 1]) {
            continue;
        }
        std::swap(data[i], data[length - i - 1]);
    }
    // reverse back the prefix O(k)/2)
    for (int i = 0; i < k/2; ++i)
    {
        if (data[i] == data[k - i - 1]) {
            continue;
        }
        std::swap(data[i], data[k - i - 1]);
    }
    // reverse back the reminder O((n - k)/2)
    for (int i = 0; i < (length - k)/2; ++i)
    {
        if (data[k + i] == data[length - i - 1]) {
            continue;
        }
        std::swap(data[k + i], data[length - i - 1]);
    }
    // essentialy this is O(n/2 + k/2 + (n-k/2)) = O(2n)
}

void rotate_array2(std::vector<int>& data, int k)
{
    size_t length = data.size();

    if (k < 0) {
        throw std::runtime_error("illegal steps value");
    }
    if (k == 0 || length == 0) {
        return;
    }
    if (k > length) {
        k = k % length;
    }

    std::vector<int> tmp; // require O(k) memory
    tmp.reserve(k);
    // copy the reminder O(k)
    for (int i = 0; i < k; ++i)
    {
        tmp[i] = data[length - k + i];
    }
    // shift prefix by k steps O(n - k)
    for (int i = 0; i < length - k; ++i)
    {
        data[i + k] = data[i];
    }
    // copy reminder back O(k)
    for (int i = 0; i < k; ++i)
    {
        data[i] = std::move(tmp[i]);
    }
    // essentialy this is O(k + n - k + k) = O(k + n) = O(n)
}

int main()
{
    std::vector<int> data = {1,2,3,4,5};
    std::cout << "before :";
    for (const auto& i : data) {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    rotate_array(data, 2);
    std::cout << "after :";
    for (const auto& i : data) {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    rotate_array2(data, 3);
    std::cout << "after2 :";
    for (const auto& i : data) {
        std::cout << i << ", ";
    }
    std::cout << std::endl;
}
