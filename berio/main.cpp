// Mark D. Berio BSCpE - 2A
// CPE 6 Objective Oriented Programming
// Exercise#1: Create a simple calculator using object oriented approach.
// Applying the following:
// Classes
// Objects
// Methods
// Inheritance
// Encapsulation and abstraction
// Polymorphism
// Encapsulation and abstraction
#include <cmath>
#include <iostream>
#include <map>
using namespace std;

class calc_oper {
public:
  virtual float compute(float op1, float op2) = 0;
  virtual ~calc_oper() {}
};

class add_oper : public calc_oper {
public:
  float compute(float op1, float op2) override { return op1 + op2; }
};

class subtract_oper : public calc_oper {
public:
  float compute(float op1, float op2) override { return op1 - op2; }
};

class mult_oper : public calc_oper {
public:
  float compute(float op1, float op2) override { return op1 * op2; }
};

class div_oper : public calc_oper {
public:
  float compute(float op1, float op2) override { return op1 / op2; }
};

class calculator {
private:
  // Private attribute
  float num1;
  float num2;
  int choice;
  std::map<int, calc_oper *> calc;

public:
  // Setter
  calculator() {
    calc[1] = new add_oper;
    calc[2] = new subtract_oper;
    calc[3] = new mult_oper;
    calc[4] = new div_oper;
  }
  ~calculator() {
    delete calc[1];
    delete calc[2];
    delete calc[3];
    delete calc[4];
  }
  float run() { return calc[choice]->compute(num1, num2); }
  int get_input() {
    cout << "\nSelect any operation from the C++ Calculator";
    cout << "\n1 = ADDITION ";
    cout << "\n2 = SUBTRACTION ";
    cout << "\n3 = MULTIPLICATION ";
    cout << "\n4 = DIVISION ";
    cout << "\n5 = Exit Program";
    cout << "\n";
    cin >> choice;
    if (choice == 5)
      return choice;
    cout << "Enter First Number: "; // print the first number
    cin >> num1;
    cout << "Enter Second Number: "; // print the second number
    cin >> num2;
    return choice;
  }
  // getter
};

int main() {
  calculator obj; // Create an object of cal
  // do while loop statement
  int option;
  while (obj.get_input() != 5) {
    cout << "The result is " << obj.run() << endl;
  };
  return 0;
}
