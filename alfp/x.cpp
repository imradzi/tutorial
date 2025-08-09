#include <stdio.h>

auto main() -> int {
  printf("Before open.\n");
  const auto f = fopen("test", "bw+");
  printf("After open.\n");
}
