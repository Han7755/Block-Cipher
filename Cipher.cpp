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
    // ki, ri 비트 분할
    Split xor_result1 = ki_1 ^ ri_1;
    Split xor_result2 = ki_1 ^ ri_2;
    // XOR 결과
    Split or_result1 = xor_result1 | ki_2;
    Split or_result2 = xor_result2 | ki_2;
    //논리합 결과
    Unit result = (or_result2 << 8) | or_result1;
    //최종 결과 합침
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
    Unit R1 = plain & 0xFFFF;
    Unit L1 = plain >> 16;
    //비트 분할
    Block key_whole[2] = {Key_rotate(key), Key_rotate(key, 2)};

    Unit k1 = key_whole[0] >> 16;
    Unit k2 = key_whole[0] & 0xFFFF;
    Unit k3 = key_whole[1] >> 16;
    Unit k4 = key_whole[1] & 0xFFFF;
    // k1, k2 저장
    Unit R2 = L1 ^ F_func(k1, R1);
    Unit L2 = R1;
    // 라운드 1
    Unit R3 = L2 ^ F_func(k2, R2);
    Unit L3 = R2;
    // 라운드 2
    Unit R4 = L3 ^ F_func(k3, R3);
    Unit L4 = R3;
    // 라운드 3
    Unit L5 = L4 ^ F_func(k4, R4);
    Unit R5 = R4;
    //라운드 4 : 바꾸지 않음
    Block result = (L5 << 16) | R5;
    //결과 합침
    return result;
}

int main()
{
    Block key = Key_rotate(0x9DD76BF3);
    unsigned short a = 0x6162;
    unsigned char b = a & 0xFF;
    unsigned char c = a >> 8;
    a = (c << 8) | b;
    cout << hex << a;
}