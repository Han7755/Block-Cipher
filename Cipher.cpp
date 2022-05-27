#include <iostream>
#include <fstream>
#include <string>
using namespace std;

typedef unsigned int Block;
typedef unsigned short Unit;
typedef unsigned char Split;

Unit F_func(Unit ki, Unit ri)
{
    // ki, ri ��Ʈ ����
    Split ki_1 = ki & 0xFF;
    Split ki_2 = ki >> 8;
    Split ri_1 = ri & 0xFF;
    Split ri_2 = ri >> 8;

    // XOR ���
    Split xor_result1 = ki_1 ^ ri_1;
    Split xor_result2 = ki_1 ^ ri_2;

    // �� ���
    Split or_result1 = (xor_result1 + ki_2) % 256;
    Split or_result2 = (xor_result2 + ki_2) % 256;

    // ���� ��� ��ħ
    Unit result = (or_result2 << 8) | or_result1;
    return result;
}

Block Key_rotate(Block key, int i = 1)
{
    if (i < 0 || i > 2)
        return 0;
    // �������� 8��Ʈ ȸ��
    key = (key << 8 * i) | (key >> (32 - (8 * i)));
    return key;
}

Block Encryption(Block key, Block plain, int Selection = 0)
{
    // ��Ʈ ����
    Unit R1 = plain & 0xFFFF;
    Unit L1 = plain >> 16;
    Block key_whole[2] = {Key_rotate(key), Key_rotate(key, 2)};

    // k1, k2, k3, k4 ����
    Unit k1 = key_whole[0] >> 16;
    Unit k2 = key_whole[0] & 0xFFFF;
    Unit k3 = key_whole[1] >> 16;
    Unit k4 = key_whole[1] & 0xFFFF;

    // ��ȣ ����
    if (Selection == 0)
    {
        // ���� 1
        Unit R2 = L1 ^ F_func(k1, R1);
        Unit L2 = R1;

        // ���� 2
        Unit R3 = L2 ^ F_func(k2, R2);
        Unit L3 = R2;

        // ���� 3
        Unit R4 = L3 ^ F_func(k3, R3);
        Unit L4 = R3;

        // ���� 4 : �ٲ��� ����
        Unit L5 = L4 ^ F_func(k4, R4);
        Unit R5 = R4;

        // ��� ��ħ
        Block result = (L5 << 16) | R5;
        return result;
    }

    // ��ȣ ����
    else
    {
        // ���� 1
        Unit R2 = L1 ^ F_func(k4, R1);
        Unit L2 = R1;
        // ���� 2

        Unit R3 = L2 ^ F_func(k3, R2);
        Unit L3 = R2;
        // ���� 3

        Unit R4 = L3 ^ F_func(k2, R3);
        Unit L4 = R3;
        // ���� 4 : �ٲ��� ����

        Unit L5 = L4 ^ F_func(k1, R4);
        Unit R5 = R4;
        // ��� ��ħ

        Block result = (L5 << 16) | R5;
        return result;
    }
}

int main()
{
    // todo���� ����� AND ECB ����
}