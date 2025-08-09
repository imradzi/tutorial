

#include <iostream>

int main() {

  int x = 1293;
  int &y = x;
  int &&z = std::move(x);
  x = 333;
  int w = x;

  std::cout << &x << std::endl;
  std::cout << &y << std::endl;
  std::cout << &z << std::endl;
  std::cout << &w << std::endl;
}
