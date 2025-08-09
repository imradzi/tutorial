#include <iostream> // for use standard I/O and some basic things
//#include "myshortcuts.hpp"

template <typename BT>
struct sLink {
    int its_data;
    sLink<BT>* its_next;
};

template <typename BT>
class cLinkList {
public:
    cLinkList()
        : its_first(nullptr)
    {
    }
    void push(const int& _new_item)
    {
        sLink<BT>* newLink = new sLink<BT>;
        newLink->its_data = _new_item;
        newLink->its_next = its_first;
        its_first = newLink;
    }

    // friend std::ostream &operator << (std::ostream &_out_obj, const
    // cLinkList<BT> &_linkList_obj);
    friend std::ostream& operator<<(std::ostream& _out_obj, const cLinkList<BT>& _linkList_obj)
    {
        sLink<BT>* current = _linkList_obj.its_first;
        while (current != nullptr) {
            std::cout << current->its_data << " ";
            current = current->its_next;
        }
        return (_out_obj);
    }

private:
    sLink<BT>* its_first;
};

/**
 * @brief the main function in the program that start from and end
 *
 * @return int , 0 successfult, other values some thing wrong happened
 */
int main()
{
    cLinkList<int> x;
    x.push(10);
    x.push(20);
    x.push(30);
    x.push(-123);
    std::cout << x << std::endl;
    return (EXIT_SUCCESS);
}
