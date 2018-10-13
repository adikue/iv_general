#include <iostream>
#include <vector>


unsigned rain(const std::vector<unsigned> profile)
{
    unsigned max_h = 0, p_sum = 0;
    for (const unsigned& h : profile) {
        p_sum += h;
        if (h > max_h) {
            max_h = h;
        }
    }
    std::cout << "max height = " << max_h << "; sum = " << p_sum << std::endl;
    unsigned water = max_h * profile.size() - p_sum;
    std::cout << "water 1 = " << water << std::endl;

    unsigned local_max = 0;
    for (int i = 0; i < profile.size(); ++i) {

        if (profile[i] > local_max) {
            local_max = profile[i];
        }
        if (local_max == max_h) {
            break;
        }
        water -= max_h - local_max;
    }
    std::cout << "water 2 = " << water << std::endl;

    local_max = 0;
    for (int i = profile.size() - 1; i > 0; --i) {

        if (profile[i] > local_max) {
            local_max = profile[i];
        }
        if (local_max == max_h) {
            break;
        }
        water -= max_h - local_max;
    }
    std::cout << "water 3 = " << water << std::endl;


    return 0;
}


int main()
{
//                                                 ||
//                                         ||      ||||  ||
//                                   __||__||||__||||||||||||
    std::vector<unsigned> profile = {0,1,0,2,1,0,1,2,2,1,2,1};

//                                   xxxxxxxxxxxxxx||xxxxxxxx
//                                   xxxxxx||xxxxxx||||xx||xx
//                                   xx||xx||||xx||||||||||||

//                                                 ||xxxxxxxx
//                                         ||xxxxxx||||xx||xx
//                                   __||xx||||xx||||||||||||


    rain(profile);
}
