#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main() {
  std::cout << "Listing of all extensions..." << std::endl;
  for (const auto &e : fs::directory_iterator(fs::current_path())) {
    if (e.is_regular_file()) {
      std::cout << e.path() << " -> ";
      if (!e.path().empty())
        std::cout << e.path().extension().string() << std::endl;
      else
        std::cout << std::endl;
    }
  }
}
