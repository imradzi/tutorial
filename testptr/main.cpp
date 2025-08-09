#include <iostream>
#include <memory>

int main() {

  struct Point {
    int x;
    int y;
  } a[4]{0, 1, 2, 3, 4, 5, 6, 7}, b[4]{5, 6, 7, 8, 9, 10, 11, 12};

  for (auto x : a) {
    std::cout << x.x << ":" << x.y << std::endl;
  }
  for (auto x : b) {
    std::cout << x.x << ":" << x.y << std::endl;
  }

  std::shared_ptr<int> ptr(new int);
  *ptr = 123;
  std::cout << ptr.get() << "-> " << *ptr << std::endl;

  std::shared_ptr<double> dPtr{};

  dPtr = nullptr;
  std::cout << "get from shared_ptr to null > " << dPtr.get() << std::endl;
  std::weak_ptr<double> x {dPtr};
  std::weak_ptr<double> y;

  std::cout << "get from weakptr to shared_ptr to null lock() = " << x.lock() << std::endl;
  std::cout << "get from weakptr to shared_ptr to null lock().get() > " << x.lock().get() << std::endl;
  std::cout << "get from weakptr to shared_ptr to empty lock().get() > " << y.lock().get() << std::endl;
}
