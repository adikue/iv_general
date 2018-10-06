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

int get_lca(node* root, int n1, int n2)
{
    if (!root) {
        return -1;
    }

    if (root->value == n1 || root->value == n2) {
        return root->value;
    }

    int l_lca = get_lca(root->left, n1, n2);
    int r_lca = get_lca(root->right, n1, n2);

    if (l_lca != -1 && r_lca != -1)
    {
        return root->value;
    } else if (l_lca != -1) {
        return l_lca;
    } else if (r_lca != -1) {
        return r_lca;
    }

    return -1;
}

int main()
{
    node* root = new_node(0);
    root->left = new_node(1);
    root->right = new_node(2);
    root->left->left = new_node(3);
    root->left->right = new_node(4);
    root->right->left = new_node(5);
    root->right->right = new_node(6);

    int a = 5, b = 4;
    std::cout << "LCA(" << a << ", " << b << ") = " << get_lca(root, a, b) << std::endl;

     //     0
     //  /    \
     //  1     2
     // / \   / \
     // 3 4   5 6
}
