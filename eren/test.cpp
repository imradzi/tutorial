#include <iostream>

int main() {
  char x = getchar();
  std::cout << x << std::endl;
  if (x == 'Y')
    std::cout << "Yes" << std::endl;
  else
    std::cout << "Not Y" << std::endl;
}
