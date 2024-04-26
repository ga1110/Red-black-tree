#include <iostream>
#include <windows.h>
#include <iomanip>
using namespace std;

HANDLE outp = GetStdHandle(STD_OUTPUT_HANDLE);
CONSOLE_SCREEN_BUFFER_INFO csbInfo;

enum Color { RED, BLACK };
struct Node {
    int data;
    Node* parent, * left, * right;
    Color color;

    Node(int data) : data(data), parent(nullptr), left(nullptr), right(nullptr), color(RED) {}
};

class RedBlackTree {
private:

    void rotateLeft(Node* node)
    {
        Node* child = node->right;
        node->right = child->left;
        if (child->left != nullptr)
        {
            child->left->parent = node;
        }
        child->parent = node->parent;
        if (node->parent == nullptr)
        {
            root = child;
        }
        else if (node == node->parent->left)
        {
            node->parent->left = child;
        }
        else
        {
            node->parent->right = child;
        }
        child->left = node;
        node->parent = child;
    }

    void rotateRight(Node* node)
    {
        Node* child = node->left;
        node->left = child->right;
        if (child->right != nullptr)
            child->right->parent = node;
        child->parent = node->parent;
        if (node->parent == nullptr)
            root = child;
        else if (node == node->parent->right)
            node->parent->right = child;
        else
            node->parent->left = child;
        child->right = node;
        node->parent = child;
    }

    void fixInsertion(Node* node) {
        while (node != root && node->parent->color == RED) {
            if (node->parent == node->parent->parent->left) {
                Node* uncle = node->parent->parent->right;
                if (uncle != nullptr && uncle->color == RED) {
                    node->parent->color = BLACK;
                    uncle->color = BLACK;
                    node->parent->parent->color = RED;
                    node = node->parent->parent;
                }
                else {
                    if (node == node->parent->right) {
                        node = node->parent;
                        rotateLeft(node);
                    }
                    node->parent->color = BLACK;
                    node->parent->parent->color = RED;
                    rotateRight(node->parent->parent);
                }
            }
            else {
                Node* uncle = node->parent->parent->left;
                if (uncle != nullptr && uncle->color == RED) {
                    node->parent->color = BLACK;
                    uncle->color = BLACK;
                    node->parent->parent->color = RED;
                    node = node->parent->parent;
                }
                else {
                    if (node == node->parent->left) {
                        node = node->parent;
                        rotateRight(node);
                    }
                    node->parent->color = BLACK;
                    node->parent->parent->color = RED;
                    rotateLeft(node->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }

    void transplant(Node* u, Node* v) {
        if (u->parent == nullptr)
            root = v;
        else if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;
        if (v != nullptr)
            v->parent = u->parent;
    }

    Node* minimum(Node* node) {
        while (node->left != nullptr)
            node = node->left;
        return node;
    }

    void fixDeletion(Node* node) {
        while (node != root && (node == nullptr || node->color == BLACK)) {
            if (node == nullptr) break; // Добавленная строка
            if (node == node->parent->left) {
                Node* sibling = node->parent->right;
                if (sibling->color == RED) {
                    sibling->color = BLACK;
                    node->parent->color = RED;
                    rotateLeft(node->parent);
                    sibling = node->parent->right;
                }
                if ((sibling->left == nullptr || sibling->left->color == BLACK) &&
                    (sibling->right == nullptr || sibling->right->color == BLACK)) {
                    sibling->color = RED;
                    node = node->parent;
                }
                else {
                    if (sibling->right == nullptr || sibling->right->color == BLACK) {
                        sibling->left->color = BLACK;
                        sibling->color = RED;
                        rotateRight(sibling);
                        sibling = node->parent->right;
                    }
                    sibling->color = node->parent->color;
                    node->parent->color = BLACK;
                    sibling->right->color = BLACK;
                    rotateLeft(node->parent);
                    node = root;
                }
            }
            else {
                Node* sibling = node->parent->left;
                if (sibling->color == RED) {
                    sibling->color = BLACK;
                    node->parent->color = RED;
                    rotateRight(node->parent);
                    sibling = node->parent->left;
                }
                if ((sibling->right == nullptr || sibling->right->color == BLACK) &&
                    (sibling->left == nullptr || sibling->left->color == BLACK)) {
                    sibling->color = RED;
                    node = node->parent;
                }
                else {
                    if (sibling->left == nullptr || sibling->left->color == BLACK) {
                        sibling->right->color = BLACK;
                        sibling->color = RED;
                        rotateLeft(sibling);
                        sibling = node->parent->left;
                    }
                    sibling->color = node->parent->color;
                    node->parent->color = BLACK;
                    sibling->left->color = BLACK;
                    rotateRight(node->parent);
                    node = root;
                }
            }
        }
        if (node != nullptr)
            node->color = BLACK;
    }

public:
    RedBlackTree() : root(nullptr) {}
    Node* root;

    void insert(int data) {
        Node* new_node = new Node(data);
        Node* current = root;
        Node* parent = nullptr;
        while (current != nullptr) {
            parent = current;
            if (data < current->data)
                current = current->left;
            else
                current = current->right;
        }
        new_node->parent = parent;
        if (parent == nullptr)
            root = new_node;
        else if (data < parent->data)
            parent->left = new_node;
        else
            parent->right = new_node;
        fixInsertion(new_node);
    }

    void remove(int data) {
        Node* node = root;
        while (node != nullptr) {
            if (data == node->data)
                break;
            else if (data < node->data)
                node = node->left;
            else
                node = node->right;
        }
        if (node == nullptr)
            return;
        Node* to_delete = node;
        Color original_color = to_delete->color;
        Node* replacement;
        if (to_delete->left == nullptr) {
            replacement = to_delete->right;
            transplant(to_delete, to_delete->right);
        }
        else if (to_delete->right == nullptr) {
            replacement = to_delete->left;
            transplant(to_delete, to_delete->left);
        }
        else {
            Node* successor = minimum(to_delete->right);
            original_color = successor->color;
            replacement = successor->right;
            if (successor->parent == to_delete)
                replacement->parent = successor;
            else {
                transplant(successor, successor->right);
                successor->right = to_delete->right;
                successor->right->parent = successor;
            }
            transplant(to_delete, successor);
            successor->left = to_delete->left;
            successor->left->parent = successor;
            successor->color = to_delete->color;
        }
        delete to_delete;
        if (original_color == BLACK)
            fixDeletion(replacement);
    }
    RedBlackTree* max_height;
};
RedBlackTree* tree = new RedBlackTree;

//////////////////////////////////////////////////
void max_height(Node* x, short& max, short deepness = 1) {
    if (deepness > max) max = deepness;
    if (x->left) max_height(x->left, max, deepness + 1);
    if (x->right) max_height(x->right, max, deepness + 1);
}
bool isSizeOfConsoleCorrect(const short& width, const short& height) {
    GetConsoleScreenBufferInfo(outp, &csbInfo);
    if (csbInfo.dwSize.X < width) {
        cout << "Please increase the width of the terminal. Width of your terminal now: "
            << csbInfo.dwSize.X << ". Minimum required: " << width << ".\n";
        return false;
    }
    return true;
}
void print_helper(Node* x, const COORD pos, const short offset) {
    SetConsoleTextAttribute(outp, x->color == RED ? 12 : 8);
    SetConsoleCursorPosition(outp, pos);
    cout << setw(offset + 1) << x->data;
    if (x->left) print_helper(x->left, { pos.X, short(pos.Y + 1) }, offset >> 1);
    if (x->right) print_helper(x->right, { short(pos.X + offset), short(pos.Y + 1) }, offset >> 1);
}
void print() {
    cout << endl;
    if (tree->root) {
        short max = 1;
        max_height(tree->root, max);
        short width = 1 << max + 1, max_w = 128; // вычисляем ширину вывода
        if (width > max_w) width = max_w;
        while (!isSizeOfConsoleCorrect(width, max)) system("pause");
        for (short i = 0; i < max; ++i) cout << '\n'; // резервируем место для вывода
        GetConsoleScreenBufferInfo(outp, &csbInfo); // получаем данные
        COORD endPos = csbInfo.dwCursorPosition;
        print_helper(tree->root, { 0, short(endPos.Y - max) }, width >> 1);
        SetConsoleCursorPosition(outp, endPos);
        SetConsoleTextAttribute(outp, 7);
    }
    cout << endl;
    for (int i = 0; i < 120; i++)
    {
        cout << ("/");
    }
    cout << endl;
}



int main() {
    tree->insert(10);
    tree->insert(20);
    tree->insert(30);
    tree->insert(40);
    tree->insert(50);
    tree->insert(60);
    tree->insert(70);
    tree->insert(80);
    tree->insert(90);
    print();

    tree->remove(20);
    print();
    return 0;
}