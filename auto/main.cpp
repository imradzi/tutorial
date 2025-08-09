#include <iostream>

constexpr const double val() { return 1.24; }

auto main() -> int {
  auto x = val();
  x += 200;
  std::cout << x << std::endl;
}
