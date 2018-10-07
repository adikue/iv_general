#include <iostream>
#include <vector>
#include <unordered_set>

std::vector<int> get_pair(const std::vector<int>& v, int sum)
{
	std::unordered_set<int> comp_set;
	for (const auto& i : v) {

		auto search = comp_set.find(i);
		if (search != comp_set.end()) {
			return {i, sum - *search};
		} else {
			comp_set.insert(sum - i);
		}
	}
	return {};
}

std::vector<int> get_pair_sorted(const std::vector<int>& v, int sum, int begin = 0)
{
    int left = begin;
    int right = v.size() - 1;

    while (left < right) {
        int test_sum = v[left] + v[right];

        if (test_sum == sum) {
            return {v[left], v[right]};
        }
        if (test_sum < sum) {
            ++left;
        } else if (test_sum > sum) {
            --right;
        }
    }
    return {};
}

std::vector<int> get_triplet_sorted(const std::vector<int>& v, int sum)
{
    int i = 0;
    while (v[i] < sum && i < v.size()) {
        int test_sum = sum - v[i];
        std::vector<int> result = get_pair_sorted(v, test_sum, i + 1);
        if (!result.empty()) {
            result.push_back(v[i]);
            return result;
        }
        ++i;
    }

    return {};
}

std::vector<int> get_ksum_sorted(const std::vector<int>& v, int sum, int k, int begin = 0)
{
    if (k == 2) {
        return get_pair_sorted(v, sum, begin);
    }

    while (v[begin] < sum && begin < v.size()) {
        int kth_sum = sum - v[begin];

        std::vector<int> result = get_ksum_sorted(v, kth_sum, k - 1, begin + 1);
        if (!result.empty()) {
            result.push_back(v[begin]);
            return result;
        }
        ++begin;
    }

    return {};

}

int main()
{
	std::vector<int> v = {4,5,6,7,2,3,1};
	int sum = 7;

	std::vector<int> p = get_pair(v, sum);
	std::cout << "pair is {";
    if (p.size()) {
        std::cout << p[0] << ":" << p[1];
    }
	std::cout << "}" << std::endl;

    std::sort(v.begin(), v.end());
    p = get_pair_sorted(v, sum);
    std::cout << "pair is {";
    if (p.size()) {
        std::cout << p[0] << ":" << p[1];
    }
    std::cout << "}" << std::endl;

    p = get_triplet_sorted(v, sum);
    std::cout << "triplet is {";
    if (p.size()) {
        std::cout << p[0] << ":" << p[1] << ":" << p[2];
    }
    std::cout << "}" << std::endl;

    p = get_ksum_sorted(v, sum, 3);
    std::cout << "triplet is {";
    if (p.size()) {
        std::cout << p[0] << ":" << p[1] << ":" << p[2];
    }
    std::cout << "}" << std::endl;

    p = get_ksum_sorted(v, 11, 4);
    std::cout << "quarta is {";
    if (p.size()) {
        std::cout << p[0] << ":" << p[1] << ":" << p[2] << ":" << p[3];
    }
    std::cout << "}" << std::endl;

}
