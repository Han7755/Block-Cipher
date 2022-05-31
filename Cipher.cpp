#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

typedef unsigned int Block;
typedef unsigned short Unit;
typedef unsigned char Split;

// F함수
Unit F_func(Unit ki, Unit ri)
{
    // ki, ri 비트 분할
    Split ki_1 = ki & 0xFF;
    Split ki_2 = ki >> 8;
    Split ri_1 = ri & 0xFF;
    Split ri_2 = ri >> 8;

    // XOR 결과
    Split xor_result1 = ki_1 ^ ri_1;
    Split xor_result2 = ki_1 ^ ri_2;

    // 합 결과
    Split or_result1 = (xor_result1 + ki_2) % 65536;
    Split or_result2 = (xor_result2 + ki_2) % 65536;

    // 최종 결과 합침
    Unit result = (or_result2 << 8) | or_result1;
    return result;
}

// 키 생성을 위한 rotate 연산기
Block Key_rotate(Block key, int i = 1)
{
    if (i < 0 || i > 2)
        return 0;
    // 좌측으로 8비트 회전
    key = (key << 8 * i) | (key >> (32 - (8 * i)));
    return key;
}

// 파일을 읽어 문자 벡터에 저장하는 함수
void LoadString(vector<Split> &Arr, string Filename)
{
    Arr.clear();
    ifstream File;
    char ch;
    File.open(Filename);
    // 파일의 문자열을 한 글자씩 벡터에 저장함(나중에 블록 단위 분할 위함)
    if (File.is_open())
    {
        while (!File.eof())
        {
            File.get(ch);
            Arr.push_back(ch);
        }
        Arr.pop_back();
        File.close();
    }
    // 예외처리
    else
        cout << "Error";
}

// 키 입력 함수
void LoadKey(string filename, Block &result)
{
    ifstream keyFile;
    string line;
    char ch;
    keyFile.open(filename);
    if (keyFile.is_open())
    {
        getline(keyFile, line);
        keyFile.close();
        result = (Block)stoi(line, nullptr, 16);
        cout << result << endl;
    }
    // 예외처리
    else
        cout << "Error";
}

// 문자열 패딩 함수
void padding(vector<Split> &Arr)
{
    int pad = Arr.size() % 4;
    int n = 4 - pad;

    // 문자열의 끝에 패딩 수와 같은 숫자로 패딩
    for (int i = 0; i < n; i++)
        Arr.push_back(n + 48);
}

// 패딩 제거 함수
void Del_padding(vector<Split> &Arr)
{
    int pad;
    if (Arr.size() % 4 != 0)
        while (Arr.size() % 4 != 0)
        {
            Arr.pop_back();
            cout << 1 << endl;
        }
    else
        for (int i = 0; i < 4; i++)
            Arr.pop_back();

    pad = Arr.back() - 48;
    for (int i = 0; i < pad; i++)
        Arr.pop_back();
}

// 읽어온 문자열을 블록 단위로 분할하는 함수
void StoB(vector<Split> &Arr, vector<Block> &block)
{
    block.clear();
    for (int i = 0; i < Arr.size(); i += 4)
    {
        // 8비트인 unsigned char를 4개 합쳐 32비트인 unsigned int로 변환
        Block input = (Arr[i] << 24) | (Arr[i + 1] << 16) | (Arr[i + 2] << 8) | Arr[i + 3];
        block.push_back(input);
    }
}

// 블록 단위의 비트를 다시 문자열로 변환하는 함수
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

// 암호/복호 함수
vector<Split> Cryption(Block key, vector<Split> Arr, int Selection = 0)
{
    vector<Block> block;
    vector<Block> result;
    padding(Arr);
    StoB(Arr, block);
    // round key 생성
    Block key_whole[2] = {Key_rotate(key), Key_rotate(key, 2)};
    // k1, k2, k3, k4 저장
    Unit k1 = key_whole[0] >> 16;
    Unit k2 = key_whole[0] & 0xFFFF;
    Unit k3 = key_whole[1] >> 16;
    Unit k4 = key_whole[1] & 0xFFFF;

    // 암호 동작
    if (Selection == 0)
    {
        for (int i = 0; i < block.size(); i++)
        {
            Unit R1 = block[i] & 0xFFFF;
            Unit L1 = block[i] >> 16;
            // 라운드 1
            Unit R2 = L1 ^ F_func(k1, R1);
            Unit L2 = R1;

            // 라운드 2
            Unit R3 = L2 ^ F_func(k2, R2);
            Unit L3 = R2;

            // 라운드 3
            Unit R4 = L3 ^ F_func(k3, R3);
            Unit L4 = R3;

            // 라운드 4 : 바꾸지 않음
            Unit L5 = L4 ^ F_func(k4, R4);
            Unit R5 = R4;

            // 결과 합침
            Block round_result = (L5 << 16) | R5;
            result.push_back(round_result);
        }
        BtoS(result, Arr);
        return Arr;
    }

    // 복호 동작
    else
    {
        for (int i = 0; i < block.size(); i++)
        {
            Unit R1 = block[i] & 0xFFFF;
            Unit L1 = block[i] >> 16;
            // 라운드 1
            Unit R2 = L1 ^ F_func(k4, R1);
            Unit L2 = R1;
            // 라운드 2

            Unit R3 = L2 ^ F_func(k3, R2);
            Unit L3 = R2;
            // 라운드 3

            Unit R4 = L3 ^ F_func(k2, R3);
            Unit L4 = R3;
            // 라운드 4 : 바꾸지 않음

            Unit L5 = L4 ^ F_func(k1, R4);
            Unit R5 = R4;
            // 결과 합침

            Block round_result = (L5 << 16) | R5;

            result.push_back(round_result);
        }
        BtoS(result, Arr);
        Del_padding(Arr);
        return Arr;
    }
}
int main()
{
    // todo파일 입출력 AND ECB 구현
    vector<Split> Arr;
    vector<Block> block;
    vector<Split> result;
    Block key;
    int z;
    LoadKey("key.txt", key);
    cout << "Select En/Decrypt (0/1): ";
    cin >> z;
    if (z == 1)
    {
        ofstream plain("plain.txt");
        LoadString(Arr, "cipher.txt");
        result = Cryption(key, Arr, 1);
        cout << result.size() << endl;
        string PT(result.begin(), result.end());
        plain << PT;
        plain.close();
        cout << "Done!" << endl;
        system("pause");
    }
    else
    {
        ofstream cipher("cipher.txt");
        string filename;
        cout << "Insert filename >>";
        cin >> filename;
        LoadString(Arr, filename);
        result = Cryption(key, Arr, 0);
        string CT(result.begin(), result.end());
        cipher << CT;
        cipher.close();
        cout << "Done!" << endl;
        system("pause");
    }
}
