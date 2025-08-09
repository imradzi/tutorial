/******************************************************************************
Author:
Date:
Code Description:


*******************************************************************************/

#include <iostream>
#include <string>

using namespace std;

class TreeNode {
public:
    int value;
    string Surname;
    string Name;

    TreeNode* left;
    TreeNode* right;

    TreeNode()
    {
        Surname = "";
        Name = "";
        value = 0;
        left = NULL;
        right = NULL;
    }

    TreeNode(int v, string sirname, string name)
    {
        value = v;
        Surname = sirname;
        Name = name;
        left = NULL;
        right = NULL;
    }
};

class AVLTree {
public:
    TreeNode* root;
    AVLTree()
    {
        root = NULL; // Set it with no contents at start.
    }

    bool isTreeEmpty()
    {
        if (root == NULL) {
            return true;

        } else {
            return false;
        }
    }

    // Get Height
    int height(TreeNode* r)
    {
        if (r == NULL)
            return -1; // initial height
        else {
            /* compute the height of each subtree */
            int lheight = height(r->left);
            int rheight = height(r->right);

            /* use the larger one */
            if (lheight > rheight)
                return (lheight + 1); // add '1' to reconcile the -1 on the initial height.
            else
                return (rheight + 1);
        }
    }

    // Get Balance factor of node N
    int getBalanceFactor(TreeNode* n)
    {
        if (n == NULL)
            return -1;
        return height(n->left) - height(n->right); // And that should range in { -1, 0, 1 }
    }

    TreeNode* rightRotate(TreeNode* y)
    {
        // Make some temporary nodes for set up the rotation to the right
        TreeNode* x = y->left;
        TreeNode* T2 = x->right;

        // Perform rotation
        x->right = y;
        y->left = T2;

        return x;
    }

    TreeNode* leftRotate(TreeNode* x)
    {
        // Make some temporary nodes for set up the rotation to the left
        TreeNode* y = x->right;
        TreeNode* T2 = y->left;

        // Perform rotation
        y->left = x;
        x->right = T2;

        return y;
    }

    TreeNode* insert(TreeNode* r, TreeNode* new_node)
    {
        if (r == NULL) {
            r = new_node;
            cout << "Employee inserted successfully" << endl;
            return r;
        }

        if (new_node->value < r->value) {
            r->left = insert(r->left, new_node);
        }

        else if (new_node->value > r->value) {
            r->right = insert(r->right, new_node);
        }

        else {
            cout << "No duplicate values allowed!" << endl;
            return r;
        }

        /*ROTATIONS OF AN AVL TREE AFTER ADDITION*/

        int bf = getBalanceFactor(r);
        // Left Left Case
        if (bf > 1 && new_node->value < r->left->value)
            return rightRotate(r);

        // Right Right Case
        if (bf < -1 && new_node->value > r->right->value)
            return leftRotate(r);

        // Left Right Case
        if (bf > 1 && new_node->value > r->left->value) {
            r->left = leftRotate(r->left);
            return rightRotate(r);
        }

        // Right Left Case
        if (bf < -1 && new_node->value < r->right->value) {
            r->right = rightRotate(r->right);
            return leftRotate(r);
        }

        /* return the (unchanged) node pointer */
        return r;
    }

    TreeNode* minValueNode(TreeNode* node)
    {
        TreeNode* current = node;
        /* loop down to find the leftmost leaf */
        while (current->left != NULL) {
            current = current->left; // traverse down
        }
        return current;
    }

    TreeNode* deleteNode(TreeNode* r, int v)
    {
        // base case
        if (r == NULL) {
            return NULL; // That means: after a value is deleted, the whole funtion call should end here.
        }
        // If the key to be deleted is smaller than the root's key,
        // then it lies in left subtree
        else if (v < r->value) {
            r->left = deleteNode(r->left, v);
        }
        // If the key to be deleted is greater than the root's key,
        // then it lies in right subtree
        else if (v > r->value) {
            r->right = deleteNode(r->right, v);
        }
        // if key is same as root's key, then This is the node to be deleted. which means its the one we are now getting to:
        else {
            // node with only one child or no child
            if (r->left == NULL) {
                TreeNode* temp = r->right;
                delete r;
                return temp;
            }

            else if (r->right == NULL) {
                TreeNode* temp = r->left;
                delete r;
                return temp;
            }

            else {
                // node with two children: Get the inorder successor (smallest
                // in the right subtree)
                TreeNode* temp = minValueNode(r->right);
                // Copy the inorder successor's content to this node
                r->value = temp->value;
                // Delete the inorder successor
                r->right = deleteNode(r->right, temp->value);
                // deleteNode(r->right, temp->value);
            }
        }

        /*ROTATIONS OF AN AVL TREE AFTER DELETION*/

        int bf = getBalanceFactor(r);
        // Left Left Imbalance/Case or Right rotation
        if (bf == 2 && getBalanceFactor(r->left) >= 0)
            return rightRotate(r);
        // Left Right Imbalance/Case or LR rotation
        else if (bf == 2 && getBalanceFactor(r->left) == -1) {
            r->left = leftRotate(r->left);
            return rightRotate(r);
        }
        // Right Right Imbalance/Case or Left rotation
        else if (bf == -2 && getBalanceFactor(r->right) <= -0)
            return leftRotate(r);
        // Right Left Imbalance/Case or RL rotation
        else if (bf == -2 && getBalanceFactor(r->right) == 1) {
            r->right = rightRotate(r->right);
            return leftRotate(r);
        }

        return r;
    }

    TreeNode* recursiveSearch(TreeNode* r, int val)
    {
        if (r == NULL || r->value == val)
            return r;

        else if (val < r->value)
            return recursiveSearch(r->left, val);

        else
            return recursiveSearch(r->right, val);
    }
};

// Drive Funtion

int main()
{
    AVLTree obj;
    string option, val;
    string surname, name;

    do {
        cout << "What operation do you want to perform? "
             << " Select Option number. Enter 0 to exit." << endl;
        cout << "1. Insert Node" << endl;
        cout << "2. Search Node" << endl;
        cout << "3. Delete Node" << endl;
        cout << "4. Clear Screen" << endl;
        cout << "0. Exit Program" << endl;

        std::getline(std::cin, option);
        // Node n1;
        TreeNode* new_node = new TreeNode();

        switch (std::stoi(option)) {
        case 0:
            break;
        case 1:
            cout << "AVL INSERT" << endl;
            cout << "Enter an Employee Number of an employee : ";
            std::getline(std::cin, val);
            new_node->value = std::atoi(val.c_str());

            cout << "Enter a Surname of an employee : ";
            std::getline(std::cin, surname);
            new_node->Surname = surname;

            cout << "Enter a name of an employee : ";
            std::getline(std::cin, name);
            new_node->Name = name;

            obj.root = obj.insert(obj.root, new_node);
            cout << endl;

            break;
        case 2: {
            cout << "SEARCH" << endl;
            cout << "Enter An Employee number to search for ";
            std::getline(std::cin, val);
            auto key = std::stoi(val);
            // new_node = obj.iterativeSearch(val);
            new_node = obj.recursiveSearch(obj.root, key);
            if (new_node != NULL) {
                cout << "Data is " << new_node->Name << " " << new_node->Surname << std::endl;
            } else {
                cout << "Employee number NOT found" << endl;
            }
            break;
        }
        case 3: {
            cout << "DELETE" << endl;
            cout << "Enter Employee number you want to delete: ";
            std::getline(std::cin, val);
            auto key = std::stoi(val);
            new_node = obj.recursiveSearch(obj.root, key);
            if (new_node != NULL) {
                obj.root = obj.deleteNode(obj.root, key);
                cout << "An employee Deleted" << endl;
            } else {
                cout << "Employee number NOT found" << endl;
            }
            break;
        }
        case 4:
            system("clear"); // clear terminal
            break;
        default:
            std::cout << "Invalid choice" << option << std::endl ;
            break;
        }

    } while (std::stoll(option) != 0);

    return 0;
}