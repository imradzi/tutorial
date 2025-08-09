#include <iostream>
#include <vector>

class Complex {
    friend std::ostream& operator<<(std::ostream& out, const Complex& x);
    int real, imaginary;

public:
    Complex(int i, int r)
      : imaginary(i),
        real(r) {
    }
};

std::ostream& operator<<(std::ostream& out, const Complex& x) {
    out << "{" << x.real << "," << x.imaginary << "}" << std::endl;
    return out;
}

int main() {
    Complex c(10, 2);
    // std::cout << "The c is " << c << std::endl;

    std::cout << "This c is ";
    ::operator<<(std::cout, c);

    std::vector<std::string> abclist {"one", "two", "three"};
    std::cout << "size of vector abclist = " << abclist.size() << std::endl;
    for (const auto& x : abclist) {
        std::cout << "\t" << x << std::endl;
    }
}
