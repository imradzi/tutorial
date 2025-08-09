#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
using namespace std;
int main() {
  vector<string> words {"this", "that", "did", "deed", "them!", "a"};
  set<char> characters;
  for_each(words.begin(), words.end(), [&characters](const string& word) {
      for_each(word.begin(), word.end(), [&characters](const char c) {
          auto [iterator, is_inserted] = characters.insert(c);
          if (is_inserted) {
              cout << c << " ";
          }
      });
  });
  cout << endl;
  return 0;
}

// int main(){
//     vector<string> words { "this", "that", "did", "deed", "them!", "a" };
//     set<char> characters;
//     cout << "[";
//     string delimiter {};
//     for (const auto& word : words) {
//         for (const auto ch : word) {
//             auto [iterator, is_inserted] = characters.insert(ch);
//             if (is_inserted) {
//                 cout << delimiter << "\"" << ch << "\"";
//                 delimiter = ", ";
//             }
//         }
//     }
//     cout << "]" << endl;
//     return 0;
// }
