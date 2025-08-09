#include <iostream>

int main() {
  std::cout << std::endl << "Output is: " << std::endl;
  int factors[3]; // uninitialized;
  std::cout << "factors[2] = " << factors[2] << std::endl;
  int f[3]{}; // initialized with default value;
  std::cout << "f[2] = " << f[2] << std::endl;
}
