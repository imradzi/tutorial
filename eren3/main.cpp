#include <algorithm>
#include <functional>
#include <iostream>
#include <unordered_map>

std::unordered_map<std::string, int> u_map { { "one", 10 }, { "two", 20 } };

class X {
public:
    std::function<int(const std::string&, const std::unordered_map<std::string, int>&)> exists;

    X()
    {
        exists = [&](const std::string& param, const std::unordered_map<std::string, int>& map) -> int {
            for (auto x : map) {
                if (x.first == param)
                    return x.second;
            }
            return -1;
        };
    }

    int get_number(const std::string& name) { return exists(name, u_map); }
};


auto main() -> int
{

    for (auto x : u_map) {
        std::cout << x.first << ", " << x.second << std::endl;
    }
    X x;
    std::cout << "one -> " << x.get_number("one") << std::endl;
    std::cout << "two -> " << x.get_number("two") << std::endl;
    std::cout << "three -> " << x.get_number("three") << std::endl;
    std::cout << std::endl;
}

/*
auto exists
    = [](const std::pair<std::string, int>& tuple) {
          for (auto& i : u_map) {
              std::cout << i.first << " check against " << tuple.first << std::endl;
              if (i.first == tuple.first) {
                  std::cout << "returns true" << std::endl;
                  return true;
              }
          }

          std::cout << "returns false" << std::endl;
          return false;
      };
*/

auto getNumber(const std::string& name, std::unordered_map<std::string, int> u_map)
{
    auto exists = [name](const std::pair<std::string, int>& tuple) {
        return tuple.first == name;
    };

    auto iter = std::find_if(u_map.begin(), u_map.end(), exists);
    
    if (iter != u_map.end())
        return iter->second;
    return -1;
}

