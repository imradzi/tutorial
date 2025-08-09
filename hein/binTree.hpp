#ifndef BINTREE_H
#define BINTREE_H
#include <fstream>
#include <iostream>
#include <stack>
#include <vector>

struct visbiilityType {
    int id;
    bool camera {false};
    bool seen {false};

	visbiilityType() : id(0), camera(false), seen(false) {}
	visbiilityType(int x): id(x), camera(false), seen(false) {}
    bool operator==(int x)
    {

        return id == x;
    }
};

template <typename type>
class binTree {
    struct binTreeNode {
        type data;
        binTreeNode* left;
        binTreeNode* right;
        binTreeNode() { left = right = nullptr; }
    };

public:
    class binTreeIterator {
    public:
        friend class binTree;
        binTreeIterator();
        binTreeIterator(binTreeNode*);
        binTreeIterator leftChild() const;
        binTreeIterator rightChild() const;

        type& operator*() const;
        bool operator==(const binTreeIterator&) const;
        bool operator!=(const binTreeIterator&) const;

    private:
        binTreeNode* binTreeNodePointer;
    };

    binTree();
    binTree(const binTree<type>&);
    const binTree& operator=(const binTree<type>&);
    ~binTree();
    void buildTree(std::vector<type>);
    binTreeIterator rootIterator() const;
    void printTree()
    {
        int count = 0;
        printPostorder(root, count);
    }

private:
    void destroyTree(binTreeNode*);
    void copyTree(binTreeNode*, binTreeNode*);
    void buildTree(std::vector<type>, binTreeNode*, int);
    void printPostorder(binTreeNode*, int);

    binTreeNode* root;
};

// Definition of member functions

template <typename type>
binTree<type>::binTreeIterator::binTreeIterator()
{
    binTreeNodePointer = NULL;
}

template <typename type>
binTree<type>::binTreeIterator::binTreeIterator(binTreeNode* ptr)
{
    binTreeNodePointer = ptr;
}

template <typename type>
typename binTree<type>::binTreeIterator binTree<type>::binTreeIterator::leftChild() const
{
    return binTreeNodePointer->left;
}

template <typename type>
typename binTree<type>::binTreeIterator binTree<type>::binTreeIterator::rightChild() const
{
    return binTreeNodePointer->right;
}

// template<typename type>
// type& binTree<type>::binTreeIterator::operator*() const
//{}

template <typename type>
bool binTree<type>::binTreeIterator::operator==(const binTree<type>::binTreeIterator& rhs) const
{
    if (&binTreeNodePointer == &rhs) {
        return true;
    }

    return false;
}

template <typename type>
bool binTree<type>::binTreeIterator::operator!=(const binTree<type>::binTreeIterator& rhs) const
{
    return binTreeNodePointer != rhs.binTreeNodePointer;
}

template <typename type>
binTree<type>::binTree()
{
    root = NULL;
}

template <typename type>
binTree<type>::binTree(const binTree<type>& copy)
{
    root = new binTreeNode;
    root->data = copy.root->data;
    root->left = copy.root->left;
    root->right = copy.root->right;
    copyTree(copy.root, copy.root);
}

// assignment operator overloading
template <typename type>
const binTree<type>& binTree<type>::operator=(const binTree<type>& rhs)
{
    root->data = rhs.root->data;
    root->left = rhs.root->left;
    root->right = rhs.root->right;
    destroyTree(rhs.root);
    return *this;
}

template <typename type>
void binTree<type>::buildTree(std::vector<type> treeValues)
{
    root = new binTreeNode;
    root->data = treeValues[0];
    root->left = NULL;
    root->right = NULL;

    buildTree(treeValues, root, 0);
}

template <typename type>
type& binTree<type>::binTreeIterator::operator*() const
{
    return binTreeNodePointer->data;
}

template <typename type>
void binTree<type>::printPostorder(binTreeNode* r, int count)
{
    if (r == NULL)
        return;

    printPostorder(r->left, ++count);
    printPostorder(r->right, ++count);

    if (r->data.id < 10) {
        std::cout << "Case  " << count << ": ";
        std::cout << r->data.id << " ";
        if (r->data.camera == 1)
            std::cout << "Camera needed\n";
        else
            std::cout << "Camera not needed\n";
    }
}

template <typename type>
binTree<type>::~binTree()
{
    // delete root;
}

template <typename type>
void binTree<type>::buildTree(std::vector<type> treeValues, binTreeNode* r, int index)
{
    // visbiilityType& abc;
    // int x = this->abc.id;

    if (index < treeValues.size()) {
        int leftIndex = (index * 2) + 1;
        int rightIndex = (index + 1) * 2;

        if (r->left == NULL) {

            if (treeValues[leftIndex].id == -1) {
                r->left = new binTreeNode;
                r->left->left = NULL;
                r->left->right = NULL;
                r->left = NULL;
            } else {

                r->left = new binTreeNode;
                r->left->left = NULL;
                r->left->right = NULL;
                treeValues[leftIndex].camera = true;
                r->left->data = treeValues[leftIndex];
            }
        }

        if (r->right == NULL) {
            if (treeValues[rightIndex].id == -1) {
                r->right = new binTreeNode;
                r->right->left = NULL;
                r->right->right = NULL;
                r->right = NULL;
            } else {
                r->right = new binTreeNode;
                r->right->left = NULL;
                r->right->right = NULL;
                treeValues[rightIndex].camera = true;
                r->right->data = treeValues[rightIndex];
            }
        }

        if (r->left != NULL)
            buildTree(treeValues, r->left, leftIndex);

        if (r->right != NULL)
            buildTree(treeValues, r->right, rightIndex);
    }
}

template <typename type>
typename binTree<type>::binTreeIterator binTree<type>::rootIterator() const
{
    return 0;
}

template <typename type>
void binTree<type>::destroyTree(binTreeNode* r)
{
    if (r == NULL) {
        return;
    }
    destroyTree(r->left);
    destroyTree(r->right);
    delete r;
}

template <typename type>
void binTree<type>::copyTree(binTreeNode* i, binTreeNode* j)
{
    if (j == NULL)
        return;

    if (j->left != NULL) {
        i->left = new binTreeNode;
        i->left->data = j->left->data;
    }

    copyTree(i->left, j->left);

    if (j->right != NULL) {
        i->right = new binTreeNode;
        i->right->data = j->right->data;
    }

    copyTree(i->right, j->right);
}
#endif
