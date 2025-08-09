// Inheritance | Encapsulation and abstraction | Polymorphism
#include <cmath>
#include <iostream>

using namespace std;

class oper {
  virtual float values(float n1, float n2) = 0;
};

class add : public oper {
public:
  float values(float n1, float n2) { return n1 + n2; }
};
class subtract : public oper {
public:
  float values(float n1, float n2) { return n1 - n2; }
};
class multiply : public oper {
public:
  float values(float n1, float n2) { return n1 * n2; }
};
class divide : public oper {
public:
  float values(float n1, float n2) {
    if (n2 == 0) {
      cout << "Division by Zero " << endl;
      return INFINITY;
    } else {
      return n1 / n2;
    }
  }
};

class calculator {
private:
  float num1;
  float num2;
  int choice;

public:
  float run() { return calc[choice]->values(num1, num2); }
  int input() {
    cout << "\n1 = ADDITION ";
    cout << "\n2 = SUBTRACTION ";
    cout << "\n3 = MULTIPLICATION ";
    cout << "\n4 = DIVISION ";
    cout << "\n5 = EXIT PROGRAM ";
    cout << "\n";

    cout << "\nEnter Choice: "; // print the enter choice in operation
    cin >> choice;
    if (choice == 5)

      return choice;
    cout << "\nEnter Choice: "; // print the enter choice in operation

    cout << "Enter First Number: "; // print the first number
    cin >> num1;
    cout << "Enter Second Number: "; // print the second number
    cin >> num2;
    return choice;
  }
};
int main() {
  calculator obj;
  while (obj.input() != 5) {
    cout << "Result = " << obj.run();
  }
}
