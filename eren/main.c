#include <stdio.h>

#define CONCAT(str1, str2) ((str1)##(str2))


int main() {
  printf("%s\n", CONCAT("Hello", "World"));
  return 0;
}
