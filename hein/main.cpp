#include "binTree.hpp"
#include <cstdlib>
#include <iostream>
#include <vector>
using namespace std;

void preorder(binTree<visbiilityType>::binTreeIterator it)
{
    binTree<visbiilityType>::binTreeIterator nil(NULL);

    if (it != nil) {
        cout << (*it).id << endl;
        preorder(it.leftChild());
        preorder(it.rightChild());
    }

    return;
}

void updateNodes(binTree<visbiilityType>::binTreeIterator it)
{
    binTree<visbiilityType>::binTreeIterator nil(NULL);

    if (it != nil) {
        (*it).id++;
        updateNodes(it.leftChild());
        updateNodes(it.rightChild());
    }

    return;
}

int main()
{
    binTree<visbiilityType> t1;
    binTree<visbiilityType> t2;

    vector<visbiilityType> v1 = { 1, 2, -1, 3, 4 };
    vector<visbiilityType> v2 = { 1, 2, -1, 3, -1, -1, -1, 4 };

    t1.buildTree(v1);
    t2.buildTree(v2);

    binTree<visbiilityType> t3(t2);

    updateNodes(t3.rootIterator());

    //t1 = t3;

    updateNodes(t1.rootIterator());

    cout << "t1" << endl;
    preorder(t1.rootIterator());
    cout << endl;

    cout << "t2" << endl;
    preorder(t2.rootIterator());
    cout << endl;

    cout << "t3" << endl;
    preorder(t3.rootIterator());
    cout << endl;

    return 0;
}
