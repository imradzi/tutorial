#include <iostream>
#include <limits>
#include <vector>

template <typename T> struct Path {
  T from;
  T to;
  int cost;
};

std::vector<Path<char>> net{{'A', 'B', 3}, {'A', 'C', 4}, {'B', 'D', 1},
                            {'B', 'F', 6}, {'C', 'D', 2}, {'C', 'E', 6},
                            {'D', 'E', 4}, {'D', 'F', 7}, {'E', 'F', 3},
                            {'E', 'G', 2}, {'F', 'G', 5}};

template <typename T> std::vector<Path<T>> findNext(T from) {
  std::vector<Path<T>> result;
  for (const auto &x : net) {
    if (x.from == from) {
      result.push_back(x);
    }
  }
  return result;
}

template <typename T>
int findShortest(T from, T to, std::vector<Path<T>> &toTake) {
  int min = std::numeric_limits<int>::max();

  std::vector<Path<T>> shortest;
  Path<T> shortestNext;
  auto adj = findNext(from);

  for (auto const &x : adj) {
    std::vector<Path<T>> tt;
    auto cost = x.cost + findShortest(x.to, to, tt);
    if (cost < min) {
      min = cost;
      shortest = tt;
      shortestNext = x;
    }
  }
  if (min < std::numeric_limits<int>::max()) {
    toTake.insert(toTake.begin(), shortest.begin(), shortest.end());
    toTake.insert(toTake.begin(), shortestNext);
    return min;
  }
  return 0;
}

int main() {
  std::vector<Path<char>> shortest;
  auto cost = findShortest('A', 'G', shortest);
  for (auto const &x : shortest) {
    std::cout << x.from << "-> " << x.to << ": " << x.cost << std::endl;
  }
  std::cout << "Total cost " << cost << std::endl;
}
