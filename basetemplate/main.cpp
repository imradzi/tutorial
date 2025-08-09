
#include <iostream>

template <typename T1> class Base {
public:
  Base() {}
  ~Base() {}

  struct Struct {
    int integer = 0;
  
    Struct(int s) : integer(s) {}
  };

  virtual void Function(Struct ns) {
      std::cout << "Base::Function integer=" << ns.integer << std::endl;
  }

  void Process(T1 tvar) {
      std::cout << "Base::Process " << tvar << std::endl;
      Struct ns(20);
      Function(ns);
  }
};

template <typename T2> class Derived : public Base<T2> {
public:
  void Function(Base<T2>::Struct ns) override {
      Base<T2>::Function(ns);
      std::cout << "Derived::Function integer=" << ns.integer << std::endl;
  }
};

int main() {

    Derived<int> dinst;
    dinst.Process(20);
}
