#include <iostream>
#include <vector>

std::vector<unsigned> give_candies(const std::vector<unsigned>& children)
{
    if (children.size() == 0) {
        return {};
    }

    std::vector<unsigned> candies(children.size());
    std::fill(candies.begin(), candies.end(), 1);

    for (int i = 1; i < children.size(); ++i) {
        if (children[i] > children[i - 1]) {
            candies[i] = std::max(candies[i - 1] + 1, candies[i]);
        }
    }
    for (int i = children.size() - 2; i >= 0; --i) {
        if (children[i] > children[i + 1]) {
            candies[i] = std::max(candies[i + 1] + 1, candies[i]);
        }
    }

    return candies;
}


int main()
{
                                   //             ||
                                   //           ||||    ||
                                   //           ||||||  ||
                                   //     ||    ||||||||||
                                   //   ||||||  ||||||||||
                                   // ||||||||||||||||||||||
    std::vector<unsigned> children = {1,2,3,2,1,5,6,4,3,5,1};
                                   //
                                   //
                                   //
                                   //     ||      ||
                                   //   ||||\\  ||||\\  ||
                                   // ||||||||||||||||||||||
    std::vector<unsigned> candies = give_candies(children);
    for (int i = 0; i < candies.size(); ++i) {
        std::cout << candies[i] << ",";
    }
    std::cout << std::endl;
}
