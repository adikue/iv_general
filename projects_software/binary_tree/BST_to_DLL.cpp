#include <iostream>

struct BSTNode
{
    int value;
    BSTNode* left;
    BSTNode* right;
};

struct DLLNode
{
    int value;
    DLLNode* next;
    DLLNode* prev;

};

BSTNode* NewBSTNode(int k)
{
    return new BSTNode{k, nullptr, nullptr};
}

DLLNode* NewDLLNode(int k)
{
    return new DLLNode{k, nullptr, nullptr};
}

DLLNode* ConvertBSTToDLL(BSTNode* bst)
{
    if (!bst) {
        return nullptr;
    }

    DLLNode* cur_node = NewDLLNode(bst->value);

    DLLNode* prev_node = ConvertBSTToDLL(bst->left);
    if (prev_node) {
        while (prev_node->next) {
            prev_node = prev_node->next;
        }
        prev_node->next = cur_node;
        cur_node->prev = prev_node;
    }

    DLLNode* next_node = ConvertBSTToDLL(bst->right);
    if (next_node) {
        next_node->prev = cur_node;
        cur_node->next = next_node;
    }

    while (cur_node->prev) {
        cur_node = cur_node->prev;
    }
    return cur_node;
}


int main()
{
    BSTNode* bst = NewBSTNode(20);
    bst->left = NewBSTNode(8);
    bst->right = NewBSTNode(22);
    bst->left->left = NewBSTNode(4);
    bst->left->right = NewBSTNode(12);
    bst->left->right->left = NewBSTNode(10);
    bst->left->right->right = NewBSTNode(14);
     //    20
     //  /    \
     //  8     22
     // / \
     // 4 12
     //  /  \
     // 10  14

    DLLNode* dll = ConvertBSTToDLL(bst);

    while (dll) {
        std::cout << dll->value;
        if (dll->next) {
            dll = dll->next;
            std::cout << "->";
        } else {
            std::cout << std::endl;
            break;
        }
    }
}
