#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#include <fmt/xchar.h>
#include <fmt/format.h>
#include <iostream>
#include <thread>
#include <nlohmann/json.hpp>

const char *xmlstring = R"V0G0N(
{
  "error": {
    "code": "ValidationError",
    "message": null,
    "target": null,
    "details": [
      {
        "code": "submission",
        "target": "submission",
        "message": "The authenticated TIN and documents TIN is not matching "
      }
    ]
  }
}
    )V0G0N";

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
    d.pop_back();  // remove last
    return GetCheckDigit(d) == chkDigit;
}

std::size_t GetThreadId(const std::thread::id id) {
    static std::size_t nextindex = 0;
    static std::mutex my_mutex;
    static std::unordered_map<std::thread::id, std::size_t> ids;
    std::lock_guard<std::mutex> lock(my_mutex);
    auto iter = ids.find(id);
    if (iter == ids.end())
        return ids[id] = nextindex++;
    return iter->second;
}

std::string toHexString(const std::string &s) {
    std::string res;
    for (auto const &x : s) {
        res.append(fmt::format("{:02X}", x));
    }
    return res;
}

int main() {
    auto id = GetThreadId(std::this_thread::get_id());
    std::cout << "thread id = " << std::this_thread::get_id() << " > " << fmt::format("{}", id) << std::endl;
    try {
        auto jv = nlohmann::json::parse(xmlstring);
        std::cout << "ERROR: " << jv["error"] << std::endl;
        for (auto &x : jv) {
            std::cout << "x = " << x << std::endl;
        }
        std::cout << "\n";

        // std::wstring wstr {L"Hello World"};
        // std::string str {"one two three four five six seven eight nine ten eleven twelve thirteen fourteen fifteen sixteen seventeen eighteen nineteen twenty"};
        // std::wcout << L"wstr = " << fmt::format(L"{} ->  {}", wstr, boost::locale::conv::utf_to_utf<wchar_t>(str)) << std::endl;

        // constexpr int digitsForReceiveIDLength = 2;
        // constexpr int digitsForPackIDLength = 1;
        // std::string receiveID {"12345"};
        // std::string packID {"111"};
        // std::cout << fmt::format("hi there, {} {}", "hello", "world") << std::endl;

        // std::cout << fmt::format(".5f = {:.5f}", 0.01) << std::endl;
        // std::cout << fmt::format(".3f = {:.3f}", 0.01) << std::endl;
        // std::cout << fmt::format("f = {:f}", 0.01) << std::endl;
        // std::cout << fmt::format("f = {:f} -> hex = {:x}", 0.01, 256) << std::endl;

        // std::cout << fmt::format("xxx = {:.{}f}", 0.1, 10) << std::endl;
        // std::cout << fmt::format("yyy = {:0{}d}", 10, 5) << std::endl;

        // std::cout << "STRING TEST: >" << fmt::format("{:>5}{:<10} {:<50}", 20, "L", "PANADOL") << "<" << std::endl;

        // // std::string num{"12345678901234567"};
        // std::string num {"1234567"};
        // std::cout << num << "->" << GetCheckDigit(num) << std::endl;
        // num.push_back(GetCheckDigit(num));
        // std::cout << "After checkdigit->" << num << std::endl;
        // std::cout << "is it ok? " << IsCheckDigitOK(num) << std::endl;

        // std::cout << "atoll('') = " << std::atoll("") << std::endl;
        // std::cout << "atoll('1234xxx') = " << std::atoll("1234xxx") << std::endl;
        // std::cout << "atoll('  1234xxx') = " << std::atoll("  1234xxx")
        //           << std::endl;
        // std::cout << "atoll('  $1234xxx') = " << std::atoll("  $1234xxx")
        //           << std::endl;

        // // std::cout << "stoll('') = " << std::atoll("") << std::endl; // --- causes
        // // exception because of empty cannot be converted to 0.
        // std::cout << "stoll('1234xxx') = " << std::stoll("1234xxx") << std::endl;
        // std::cout << "stoll('  1234xxx') = " << std::stoll("  1234xxx")
        //           << std::endl;
        // std::cout << "stoll('  $1234xxx') = " << std::stoll("  $1234xxx")
        //           << std::endl;

        // std::cout << "\n\n\n";

    } catch (std::exception &e) {
        std::cout << "\nException : " << e.what() << std::endl;
    }
    std::cout << "thread id = " << id << " > " << fmt::format("{}", id) << std::endl;
    std::cout << " 123 = > " << fmt::format("{:05d}", 123) << std::endl;
    std::string s = "Saya Sayang awak 0123";
    std::cout << "Format to hex: " << s << " -> " << toHexString(s) << std::endl;
}
