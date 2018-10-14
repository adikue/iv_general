#include <iostream>

struct Node
{
    int value;
    Node* left;
    Node* right;
};

int count_univals(const Node* root)
{
    if (!root) {
        return 0;
    }
    int result = count_univals(root->left);
    result += count_univals(root->right);

    bool unival = true;
    if (root->left && root->value != root->left->value) {
        unival = false;
    }
    if (root->right && root->value != root->right->value) {
        unival = false;
    }
    if (unival) {
        result += 1;
    }

    return result;
}


int main()
{
    //  0
    // / \
    // 1  0
    //   / \
    //   1 0
    //  / \
    //  1 1
    Node* root = new Node{0, nullptr, nullptr};
    root->left = new Node{1, nullptr, nullptr};
    root->right = new Node{0, nullptr, nullptr};
    root->right->left = new Node{1, nullptr, nullptr};
    root->right->right = new Node{0, nullptr, nullptr};
    root->right->left->left = new Node{1, nullptr, nullptr};
    root->right->left->right = new Node{1, nullptr, nullptr};

    std::cout << count_univals(root) << std::endl;
}
