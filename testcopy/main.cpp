#include <iostream>
struct C {
  C() {}
  C(const C &) { std::cout << "A copy was made.\n"; }
};

C f() { return C(); }

C g() {
  C c;
  return c;
}

int main() {
  std::cout << "Hello World!\n";
  C obj = f();
  std::cout << "g" << std::endl;
  C o2 = g();
}
