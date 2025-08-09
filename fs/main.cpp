#include <boost/algorithm/string/replace.hpp>
#include <boost/tokenizer.hpp>
#include <chrono>
#include <filesystem>
#include <fmt/format.h>
#include <iostream>
#include <vector>
using namespace std::chrono_literals;

inline std::string FormatDate(const std::chrono::system_clock::time_point &tp,
                              const char *format = "%F %T %Z") {
  std::time_t s_time = std::chrono::system_clock::to_time_t(tp);
  std::ostringstream oss;
  oss << std::put_time(std::localtime(&s_time), format);
  return oss.str();
}

void tokenize(const std::string &t) {
  boost::tokenizer<boost::char_separator<char>> tok(
      t, boost::char_separator<char>(";- ", "", boost::keep_empty_tokens));
  for (auto &x : tok) {
    std::cout << x << std::endl;
  }
}

namespace fs = std::filesystem;
void CreateNonExistingFolders(const std::string &filename) {
  std::vector<std::string> plist;
  for (auto &e : fs::path{filename}) {
    plist.push_back(e.generic_string());
  }
  plist.pop_back();
  std::string path;
  for (auto &e : plist) {
    path.append(e);
    if (e.at(0) != fs::path::preferred_separator)
      path.push_back(fs::path::preferred_separator);
  }
  std::cout << path << std::endl;
  fs::create_directories(path);
}

typedef std::chrono::system_clock::time_point TimePoint;

int main() {
  namespace fs = std::filesystem;

  std::cout << "current path = " << fs::current_path() << std::endl;
  std::cout << "temp path = " << fs::temp_directory_path() << std::endl;

  CreateNonExistingFolders(
      "/home/radzi/cmake/tutorial/fs000/testing111/testing2222/CMakeLists.txt");

  for (auto &e : fs::directory_iterator(fs::current_path())) {
    std::cout << e << "extension = " << e.path().extension() << std::endl;
  }

  fs::path path{"/home/radzi/cmake/tutorial-xyz/fs/build/456.zip"};
  std::cout << path.string() << std::endl;
  std::cout << "root_name = " << path.root_name().string() << std::endl;
  std::cout << "root_directory = " << path.root_directory().string()
            << std::endl;
  std::cout << "root_path = " << path.root_path().string() << std::endl;
  std::cout << "relative_path = " << path.relative_path().string() << std::endl;
  std::cout << "parent_path = " << path.parent_path().string() << std::endl;
  std::cout << "filename = " << path.filename().string() << std::endl;
  std::cout << "stem = " << path.stem().string() << std::endl;
  std::cout << "extension = " << path.extension().string() << std::endl;

  auto dob = std::chrono::sys_days{1962y / 11 / 28};
  std::cout << "dob = " << FormatDate(dob) << std::endl;

  TimePoint tn = TimePoint::clock::now();
  double ndays =
      std::chrono::duration_cast<std::chrono::days>(tn - dob).count();
  double nmonth =
      std::chrono::duration_cast<std::chrono::months>(tn - dob).count();
  double nyear =
      std::chrono::duration_cast<std::chrono::years>(tn - dob).count();

  std::cout << fmt::format("Dob = {}: age = {} year, {} month, {} days",
                           FormatDate(dob), nyear, nmonth, ndays)
            << std::endl;

  auto age = tn - dob;
  auto ryear = std::chrono::floor<std::chrono::years>(age);
  auto rmonth = std::chrono::floor<std::chrono::months>(age - ryear);
  auto rday = std::chrono::floor<std::chrono::days>(age - ryear - rmonth);

  std::cout << ryear.count() << ", " << rmonth.count() << ", " << rday.count()
            << std::endl;

  tokenize("testing;1245----456 zzzzz");

  std::cout << "---------------------" << std::endl;

  std::string p = std::filesystem::current_path();
  std::string x = p;
  x.push_back(std::filesystem::path::preferred_separator);
  x.append("myfilename.txt");
  std::cout << p << std::endl;
  std::cout << x << std::endl;
  std::cout << boost::replace_first_copy(x, p, "-->") << std::endl;
  std::cout << boost::replace_first_copy(x, p, "") << std::endl;
}
