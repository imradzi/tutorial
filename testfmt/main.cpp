#include <boost/algorithm/string.hpp>
#include <fmt/format.h>
#include <iostream>

char GetCheckDigit(const std::string &d) {
  bool isEven = true;
  int sumEven = 0, sumOdd = 0;
  for (auto it = d.begin(); it != d.end(); it++, isEven = !isEven) {
    if (isEven)
      sumEven += (char)*it - '0';
    else
      sumOdd += (char)*it - '0';
  }
  int mod = (sumEven + sumOdd * 3) % 10;
  if (mod != 0)
    mod = 10 - mod;
  return char(mod + '0');
}

bool IsCheckDigitOK(const std::string &str) {
  auto d = boost::trim_copy(str);
  if (d.empty())
    return false;
  char chkDigit = d.back();
  d.pop_back(); // remove last
  return GetCheckDigit(d) == chkDigit;
}

int main() {
  try {
    constexpr int digitsForReceiveIDLength = 2;
    constexpr int digitsForPackIDLength = 1;
    std::string receiveID{"12345"};
    std::string packID{"111"};
    std::cout << fmt::format("hi there, {} {}", "hello", "world") << std::endl;

    // std::string num{"12345678901234567"};
    std::string num{"1234567"};
    std::cout << num << "->" << GetCheckDigit(num) << std::endl;
    num.push_back(GetCheckDigit(num));
    std::cout << "After checkdigit->" << num << std::endl;
    std::cout << "is it ok? " << IsCheckDigitOK(num) << std::endl;

    std::cout << "atoll('') = " << std::atoll("") << std::endl;
    std::cout << "atoll('1234xxx') = " << std::atoll("1234xxx") << std::endl;
    std::cout << "atoll('  1234xxx') = " << std::atoll("  1234xxx")
              << std::endl;
    std::cout << "atoll('  $1234xxx') = " << std::atoll("  $1234xxx")
              << std::endl;

    // std::cout << "stoll('') = " << std::atoll("") << std::endl; // --- causes
    // exception because of empty cannot be converted to 0.
    std::cout << "stoll('1234xxx') = " << std::stoll("1234xxx") << std::endl;
    std::cout << "stoll('  1234xxx') = " << std::stoll("  1234xxx")
              << std::endl;
    std::cout << "stoll('  $1234xxx') = " << std::stoll("  $1234xxx")
              << std::endl;
  } catch (std::exception &e) {
    std::cout << "Exception : " << e.what() << std::endl;
  }
}
