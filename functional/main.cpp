#include <functional>
#include <iostream>

class Foo {
public:
  int val;
  Foo(int v) : val(v) {}
  void foo() const { std::cout << val << std::endl; }
};

class Bar {
public:
  void foo() const { std::cout << "From Bar" << std::endl; }
};

int main() {

  Bar bar;
  std::function<void(void)> f1 = [bar]() { bar.foo(); };

  f1();

  Foo y(20);

  f1 = [y]() { y.foo(); };

  y.val = 30;

  std::function<void(void)> f2 = [y]() { y.foo(); };

  std::cout << "f1 ";
  f1(); // remembers the value of y at the creation of the lambda  expression ->
        // 20
  std::cout << "f2 ";
  f2();
}
