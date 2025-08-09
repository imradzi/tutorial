#include <stdio.h>

int c_main() {
  int num, loop = 5678;
  printf("type your number:");
  scanf("%d", &num);

  while (num != loop) {
    printf("wrong number! type again.\n\n");
  AAYUSH:
    printf("type your number:");
    scanf("%d", &num);

    if (num == 111)
      goto AAYUSH;
    if (num == 123) {
      printf("generic\n\n");
      goto AAYUSH;
    }

    if (num == loop) {
      printf("congratulations");
      break;
    }
  }
  return 0;
}

int main() {
  int num;
  const int loop = 5678;
  while (num != loop) {
    printf("type your number:");
    scanf("%d", &num);
    switch (num) {
    case loop:
      printf("congratulations");
      break;
    case 111:
      break;
    case 123:
      printf("generic\n\n");
      break;
    default:
      printf("wrong number! type again.\n\n");
      break;
    }
  }
  return 0;
}
