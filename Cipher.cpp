#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

typedef unsigned int Block;
typedef unsigned short Unit;
typedef unsigned char Split;

// F�Լ�
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

// Ű ������ ���� rotate �����
Block Key_rotate(Block key, int i = 1)
{
    if (i < 0 || i > 2)
        return 0;
    // �������� 8��Ʈ ȸ��
    key = (key << 8 * i) | (key >> (32 - (8 * i)));
    return key;
}

Block Cryption(Block key, Block plain, int Selection = 0)
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

// ������ �о� ���� ���Ϳ� �����ϴ� �Լ�
void LoadString(vector<Split> &Arr)
{
    ifstream File;
    char ch;
    File.open("test.txt");
    // ������ ���ڿ��� �� ���ھ� ���Ϳ� ������(���߿� ��� ���� ���� ����)
    if (File.is_open())
    {
        while (!File.eof())
        {
            File.get(ch);
            Arr.push_back(ch);
        }
        File.close();
    }
    // ����ó��
    else
        cout << "Error";
}

// ���ڿ� �е� �Լ�
void padding(vector<Split> &Arr)
{
    int pad = Arr.size() % 4;
    int n = 4 - pad;

    // ���ڿ��� ���� �е� ���� ���� ���ڷ� �е�
    for (int i = 0; i < n; i++)
        Arr.push_back(n + 48);
}

// �о�� ���ڿ��� ��� ������ �����ϴ� �Լ�
void StoB(vector<Split> &Arr, vector<Block> &block)
{
    block.clear();
    for (int i = 0; i < Arr.size(); i += 4)
    {
        // 8��Ʈ�� unsigned char�� 4�� ���� 32��Ʈ�� unsigned int�� ��ȯ
        Block input = (Arr[i] << 24) | (Arr[i + 1] << 16) | (Arr[i + 2] << 8) | Arr[i + 3];
        block.push_back(input);
    }
}

// ��� ������ ��Ʈ�� �ٽ� ���ڿ��� ��ȯ�ϴ� �Լ�
void BtoS(vector<Block> &block, vector<Split> &Arr)
{
    Arr.clear();
    for (int i = 0; i < block.size(); i++)
        for (int j = 0; j < 4; j += 4)
        {
            Split input1 = block[i] >> 24;
            Split input2 = (block[i] >> 16) & 0xFF;
            Split input3 = (block[i] >> 8) & 0xFF;
            Split input4 = (block[i] & 0xFF);
            Arr.push_back(input1);
            Arr.push_back(input2);
            Arr.push_back(input3);
            Arr.push_back(input4);
        }
}

int main()
{
    // todo���� ����� AND ECB ����
    vector<Split> Arr;
    vector<Block> block;
    LoadString(Arr);
    for (int i = 0; i < Arr.size(); i++)
        cout << Arr[i];
    cout << Arr.size() << endl;
    padding(Arr);
    cout << Arr.size() << endl;
    cout << Arr[403]
         << endl;
    StoB(Arr, block);
    for (int i = 0; i < block.size(); i++)
        cout << block[i] << endl;
}
