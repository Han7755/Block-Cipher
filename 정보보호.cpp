#include <iostream>
#define ROUND 4
using namespace std;

typedef unsigned int Block;
typedef unsigned short Unit;
typedef unsigned char Split;

Unit F_func(Unit ki, Unit ri)
{
    Split ki_1 = ki & 0xFF;
    Split ki_2 = ki >> 8;
    Split ri_1 = ri & 0xFF;
    Split ri_2 = ri >> 8;

    Split xor_result1 = ki_1 ^ ri_1;
    Split xor_result2 = ki_1 ^ ri_2;

    Split or_result1 = xor_result1 | ki_2;
    Split or_result2 = xor_result2 | ki_2;

    Unit result = (or_result2 << 8) | or_result1;
    return result;
}

Block Key_rotate(Block key, int i = 1)
{
    if (i < 0 || i > 2)
        return 0;
    key = (key << 8 * i) | (key >> (32 - (8 * i)));
    return key;
}

Block Encryption(Block key, Block plain)
{
}

int main()
{
    Block key = Key_rotate(0x9DD76BF3);
    unsigned short a = 0x6162;
    unsigned char b = a & 0xFF;
    unsigned char c = a >> 8;
    cout << c << b;
}