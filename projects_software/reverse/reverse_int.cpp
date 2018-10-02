#include <iostream>

template<typename T>
T reverse_int(T x, int base = 10)
{
    T ret = 0;
    while (x) {
        T main = ret * base;
        if (x > 0 && main < ret) {
            throw std::range_error("overflow");
        }
        ret = main + x % base;
        x = x / base;
    }
    return ret;
}

int main()
{
    long a = 0xdeadbeef;
    std::cout << "before: " << std::hex << a << std::endl;
    a = reverse_int(a, 16);
    std::cout << "after: " << std::hex << a << std::endl;
}
