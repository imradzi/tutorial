#include <iostream>

int getNoOfBits(int n) {
    int i = 0;
    for (; n > 0; i++) {
        n = n >> 1;
    }
    return i;
}

int firstBit(int n) {
    return n & 0x01;
}

int lastBit(int n) {
    int nBits = getNoOfBits(n);
    return n >> (nBits - 1);
}

#include <bitset>
int takeLandFsetbits(int n) {
    std::cout << std::bitset<8 * sizeof(n)>(n) << std::endl;
    n |= n >> 1;
    std::cout << std::bitset<8 * sizeof(n)>(n) << std::endl;
    n |= n >> 1;
    std::cout << std::bitset<8 * sizeof(n)>(n) << std::endl;
    n |= n >> 1;
    std::cout << std::bitset<8 * sizeof(n)>(n) << std::endl;
    n |= n >> 1;
    std::cout << std::bitset<8 * sizeof(n)>(n) << std::endl;
    n |= n >> 1;
    std::cout << std::bitset<8 * sizeof(n)>(n) << std::endl;

    return ((n + 1) >> 1) + 1;
}

int toggleFanddLbits(int n) {
    if (n == 1) return 0;
    return n ^ takeLandFsetbits(n);
}

int main(int argc, const char *argv[]) {
    int n = std::atoi(argv[1]);
    int nBits = getNoOfBits(n);
    std::cout << "Length of n = " << nBits << std::endl;
    std::cout << "LSB Bit = " << firstBit(n) << std::endl;
    std::cout << "MSB Bit = " << lastBit(n) << std::endl;

    int res = 1;
    for (int i = 0; i < nBits - 1; i++) {
        res = res << 1 | 1;
    }
    std::cout << std::bitset<8 * sizeof(n)>(n) << " ----> " << std::bitset<8 * sizeof(res)>(res) << std::endl;
}