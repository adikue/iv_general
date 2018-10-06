#include <iostream>
#include <vector>

struct node {
	int value;
	node* left;
	node* right;
};

node* new_node(int k)
{
	return new node{k, nullptr, nullptr};
}

bool in_bst(node* root, int key)
{
    if (!root) {
        return false;
    }

    if (key == root->value) {
        return true;
    }
    if (key < root->value) {
        return in_bst(root->left, key);
    }
    return in_bst(root->right, key);
}

int get_lca(node* root, int n1, int n2)
{
    if (!root) {
        return -1;
    }

    if (root->value > n1 && root->value > n2) {
        return get_lca(root->left, n1, n2);
    } else if (root->value < n1 && root->value < n2) {
        return get_lca(root->right, n1, n2);
    }

    if (!in_bst(root, n1) || !in_bst(root, n2)) {
        return -1;
    }

    return root->value;
}

int main()
{
    node* root = new_node(20);
    root->left = new_node(8);
    root->right = new_node(22);
    root->left->left = new_node(4);
    root->left->right = new_node(12);
    root->left->right->left = new_node(10);
    root->left->right->right = new_node(14);

    int a = 4, b = 8;
    std::cout << "LCA(" << a << ", " << b << ") = " << get_lca(root, a, b) << std::endl;

     //    20
     //  /    \
     //  8     22
     // / \
     // 4 12
     //  /  \
     // 10  14
}
