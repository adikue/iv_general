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

struct DLL
{
    DLLNode* start;
    DLLNode* end;
};

DLL* ConvertBSTToDLL(BSTNode* bst)
{
    if (!bst) {
        return nullptr;
    }

    DLLNode* cur_node = NewDLLNode(bst->value);

    DLL* prev_list = ConvertBSTToDLL(bst->left);
    if (prev_list) {
        prev_list->end->next = cur_node;
        cur_node->prev = prev_list->end;
    } else {
        prev_list = new DLL{cur_node, nullptr};
    }

    DLL* next_list = ConvertBSTToDLL(bst->right);
    if (next_list) {
        next_list->start->prev = cur_node;
        cur_node->next = next_list->start;
    } else {
        next_list = new DLL{nullptr, cur_node};
    }

    return new DLL{prev_list->start, next_list->end};
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

    DLLNode* dll = ConvertBSTToDLL(bst)->start;

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
