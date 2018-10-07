#include <iostream>
#include <vector>
#include <unordered_set>

using Board = std::vector<std::vector<int>>;

constexpr int k_valid_sum = 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9;

bool is_duplicate(std::unordered_set<int>& check_set, int num)
{
    if (num == 0) {
        return false;
    }
    if (check_set.count(num)) {
        return true;
    }
    check_set.insert(num);
    return false;
};

int check_row(const Board& board, int row_num)
{
    std::unordered_set<int> check_set;
    int sum = 0;

    for (const int& num : board[row_num]) {
        if (is_duplicate(check_set, num)) {
            return -1;
        }
        sum += num;
    }

    return sum;
}

int check_col(const Board& board, int col_num)
{
    std::unordered_set<int> check_set;
    int sum = 0;

    for (int i = 0; i < 9; ++i) {
        int num = board[i][col_num];
        if (is_duplicate(check_set, num)) {
            return -1;
        }
        sum += num;
    }

    return sum;
}

int check_cell(const Board& board, int cell_num_x, int cell_num_y)
{
    std::unordered_set<int> check_set;
    int sum = 0;

    for (int i = cell_num_x * 3; i < (cell_num_x + 1) * 3; ++i) {
        for (int j = cell_num_y * 3; j < (cell_num_y + 1) * 3; ++j) {
            int num = board[i][j];
            if (is_duplicate(check_set, num)) {
                return -1;
            }
            sum += num;
        }
    }

    return sum;
}

bool is_valid_number(const Board& board, int x, int y)
{
    if (board[x][y] > 9) {
        return false;
    }

    if (check_row(board, x) == -1) {
        return false;
    }
    if (check_col(board, y) == -1) {
        return false;
    }

    int cell_x_num = x / 3;
    int cell_y_num = y / 3;
    if (check_cell(board, cell_x_num, cell_y_num) == -1) {
        return false;
    }

    return true;
}

bool is_board_solved(const Board& board)
{
    for (int i = 0; i < 9; ++i) {
        if (check_row(board, i) != k_valid_sum){
            return false;
        }
        if (check_col(board, i) != k_valid_sum){
            return false;
        }

        int cell_x_num = i % 3;
        int cell_y_num = i / 3;
        if (check_cell(board, cell_x_num, cell_y_num) != k_valid_sum) {
            return false;
        }
    }
    return true;
}

int solve(Board& board, int start_x, int start_y)
{
    int x = start_x, y = start_y;
    bool found = false;
    for (; x < 9; ++x) {
        for (; y < 9; ++y) {
            if (board[x][y] == 0) {
                found = true;
                break;
            }
        }
        if (found) {
            break;
        } else {
            y = 0;
        }
    }

    if (!found) {
        if (!is_board_solved(board)) {
            return -1;
        }
        return 0;
    }

    int i;
    for (i = 1; i <= 9; ++i) {
        board[x][y] = i;
        if (!is_valid_number(board, x, y)) {
            continue;
        }
        int status = solve(board, x, y);
        if (status == 0) {
            return status;
        }
    }
    if (i == 10) {
        board[x][y] = 0;
        return -1;
    }
    return 0;
}

void print_board(const Board& board)
{
    for (int x = 0; x < 9; ++x) {
        for (int y = 0; y < 9; ++y) {
            if (y % 3 == 0) {
                std::cout << " ";
            }

            std::string symb = board[x][y] ? std::to_string(board[x][y]) : "_";
            std::cout << symb << " " ;
        }
        std::cout << std::endl;
        if (x % 3 == 2) {
            std::cout << std::endl;
        }
    }
}

int main()
{
    Board board = {
        {3,9,0, 6,0,0, 0,0,0},
        {0,7,5, 8,0,2, 3,0,0},
        {8,6,0, 0,0,0, 0,9,4},

        {0,0,7, 3,1,0, 9,8,0},
        {0,5,0, 0,4,0, 0,2,0},
        {0,3,9, 0,2,8, 4,0,0},

        {7,2,0, 0,0,0, 0,6,8},
        {0,0,3, 2,0,5, 1,4,0},
        {0,0,0, 0,0,7, 0,0,9},
    };

    print_board(board);

    int is_solved = solve(board, 0, 0);
    std::cout << (is_solved == 0 ? "solved" : "not solved") << std::endl;

    print_board(board);
}
