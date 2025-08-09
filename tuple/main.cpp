#include <fmt/format.h>
#include <iostream>
#include <string>
#include <vector>

auto fnTuple() -> std::tuple<int, std::vector<std::string>> {
  int r = 10;
  std::vector<std::string> astring{"a", "b", "c"};

  return {r, astring};
}

int main() {
  auto [retInt, retVector] = fnTuple();

  std::cout << retInt << std::endl;
  for (auto x : retVector) {
    std::cout << x << std::endl;
  }
}
