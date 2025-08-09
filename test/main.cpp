#include <iostream>
#include <boost/tokenizer.hpp>

int main() {
    std::string key = "d8a64004/e1786fcb//2355";
    boost::tokenizer<boost::char_separator<char>> tok(key, boost::char_separator<char>("/", "", boost::keep_empty_tokens));
    int i = 0;
    std::string folder, dbname, chatId, billId;
    for (auto it = tok.begin(); it != tok.end(); it++, i++) {
        switch (i) {
            case 0: folder = *it; break;
            case 1: dbname = *it; break;
            case 2: chatId = *it; break;
            case 3: billId = *it; break;
        }
    }
    std::cout << "FROM data: " << key << std::endl;
    std::cout << "folder : " << folder << std::endl;
    std::cout << "dbname : " << dbname << std::endl;
    std::cout << "chatId : " << chatId << std::endl;
    std::cout << "billId : " << billId << std::endl;
}