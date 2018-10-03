#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>

static int wd_count;


int words_distance(std::string first, std::string second)
{
    ++wd_count;
    if (first.size() != second.size()) {
        throw std::runtime_error("words have different lehgth");
    }

    int distance = 0;
    for (int i = 0; i < first.size(); ++i) {
        if (first[i] != second[i]) {
            ++distance;
        }
    }

    return distance;
 }


int find(const std::string& start, const std::string& end, std::unordered_set<std::string>& dict, int start_lvl)
{
    std::vector<std::string> nexts;

    int found = 0;
    std::string found_next;

    if (words_distance(start, end) == 1) {
        return start_lvl + 1;
    }

    for (const auto& dict_word : dict) {
        if (words_distance(start, dict_word) == 1) {
            nexts.push_back(dict_word);
        }
    }

    for (const auto next : nexts) {
        dict.erase(next);
        int new_found = find(next, end, dict, start_lvl + 1);
        if (new_found && (found == 0 || new_found < found)) {
            found = new_found;
        }
    }

    return found;
}


int main()
{
    std::string start = "hit";
    std::string end = "cog";
    std::unordered_set<std::string> dict = {"hot","dot","dog","lot","log"};
    int dl = dict.size();

    int min_path = find(start, end, dict, 0);
    std::cout << "min permutations = " << min_path << std::endl;
    std::cout << "dict size = " << dl << "; iterations = " << wd_count << std::endl;
}
