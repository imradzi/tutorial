#include <iostream>
struct Obj {
  int i;

public:
  Obj(int d) : i(d) { std::cout << "constructor" << std::endl; }
  Obj(const Obj &x) {
    std::cout << "copy constructor" << std::endl;
    i = x.i;
  }
};

auto MakeObj() {
  Obj o(200);
  return o;
}

Obj obj(2);
int main() {
  obj = MakeObj();
  obj = MakeObj();
}
