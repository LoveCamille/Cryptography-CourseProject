//
//  main.c
//  彩虹表
//
//  Created by 蒋🌲齐 on 2020/10/10.
//  Copyright © 2020 蒋🌲齐. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>

#define SHA1_ROTL(a,b) (SHA1_tmp=(a),((SHA1_tmp>>(32-b))&(0x7fffffff>>(31-b)))|(SHA1_tmp<<b))
#define SHA1_F(B,C,D,t) ((t<40)?((t<20)?((B&C)|((~B)&D)):(B^C^D)):((t<60)?((B&C)|(B&D)|(C&D)):(B^C^D)))

const unsigned v1[76] = { 0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36 };
const char v2[36] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z' };

unsigned int SHA1_tmp;

using namespace std;

//声明
void Hash_Sha1(unsigned *sha1);
void getstr(unsigned n, char str[8]);
void R(const unsigned sha1[5], char str[8], int i);

int Iterate(unsigned step_verify, char *cur_chainhead);
int StrSHA1(const char* str, int length, unsigned sha1[5]);

unsigned s2ui(char *str);
unsigned Hash(char *str, int i);

//down⬇️
int StrSHA1(const char* str, int length, unsigned sha1[5]) {
    /*
    计算字符串SHA-1
    参数说明：
    str         字符串指针
    length      字符串长度
    sha1         用于保存SHA-1的字符串指针
    返回值为参数sha1
    */
    unsigned char* pp, * ppend;
    unsigned int l, i, K[80], W[80], TEMP, A, B, C, D, E, H0, H1, H2, H3, H4;
    H0 = 0x67452301, H1 = 0xEFCDAB89, H2 = 0x98BADCFE, H3 = 0x10325476, H4 = 0xC3D2E1F0;
    for (i = 0; i < 20; K[i++] = 0x5A827999);
    for (i = 20; i < 40; K[i++] = 0x6ED9EBA1);
    for (i = 40; i < 60; K[i++] = 0x8F1BBCDC);
    for (i = 60; i < 80; K[i++] = 0xCA62C1D6);
    l = length + ((length % 64 > 56) ? (128 - length % 64) : (64 - length % 64));
    if (!(pp = (unsigned char*)malloc((unsigned int)l))) return -1;
    for (i = 0; i < length; pp[i + 3 - 2 * (i % 4)] = str[i], i++);
    for (pp[i + 3 - 2 * (i % 4)] = 128, i++; i < l; pp[i + 3 - 2 * (i % 4)] = 0, i++);
    *((unsigned int*)(pp + l - 4)) = length << 3;
    *((unsigned int*)(pp + l - 8)) = length >> 29;
    for (ppend = pp + l; pp < ppend; pp += 64)
    {
        for (i = 0; i < 16; W[i] = ((unsigned int*)pp)[i], i++);
        for (i = 16; i < 80; W[i] = SHA1_ROTL((W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16]), 1), i++);
        A = H0, B = H1, C = H2, D = H3, E = H4;
        for (i = 0; i < 80; i++)
        {
            TEMP = SHA1_ROTL(A, 5) + SHA1_F(B, C, D, i) + E + W[i] + K[i];
            E = D, D = C, C = SHA1_ROTL(B, 30), B = A, A = TEMP;
        }
        H0 += A, H1 += B, H2 += C, H3 += D, H4 += E;
    }
    free(pp - l);
    sha1[0] = H0, sha1[1] = H1, sha1[2] = H2, sha1[3] = H3, sha1[4] = H4;
    return 0;
}

void R(const unsigned sha1[5], char str[8], int i) {
    getstr((sha1[0] + sha1[1] * i) % 2176782336, str);
}

void getstr(unsigned n, char str[8]) {
    str[0] = 'a'; str[1] = '0'; str[2] = '0'; str[3] = '0'; str[4] = '0'; str[5] = '0'; str[6] = '0'; str[7] = '0';
    int i = 2;
    while (n) {
        unsigned tmp = n % 36;
        str[i++] = v2[tmp];
        n = n / 36;
    }
}

unsigned s2ui(char *str) {
    unsigned res = 0;
    unsigned tmp = 0;
    unsigned pn = 1;
    for (int i = 2; i < 8; i++) {
        tmp = v1[str[i] - '0'];
        res += tmp * pn;
        pn *= 36;
    }
    return res;
}

unsigned Hash(char *str, int i) {
    unsigned sha1[5];
    StrSHA1(str, 8, sha1);
    R(sha1, str, i);
    return s2ui(str);
}

void Hash_Sha1(unsigned *sha1) {
    char Plaintext_C[8];
    for (int j = 0; j < 5; j++) {
        sha1[j] = 0;
        for (int i = 0; i < 8; i++) {
            Plaintext_C[i] = getchar();
            sha1[j] = (sha1[j] << 4) | (v1[Plaintext_C[i] - '0']);
        }
    }
}

int Iterate(unsigned step_verify, char *cur_chainhead) {
    int flag = 0;
    char test[9];
    unsigned pre_step, step = 0;
    strncpy(test, cur_chainhead, 9);
    for (int i = 1; i <= 1000; i++) {
        for (int j = 1; j <= 100; j++) {
            pre_step = step;
            step = Hash(test, j);
            if (step == step_verify) {
                flag = 1;
                break;
            }
        }
        if (flag) {
            getstr(pre_step, test);
            test[8] = 0;
            cout << test;
            break;
        }
    }
    return flag;
}

int main() {
    unsigned  N;
    scanf("%d", &N);
    
    char password[9]={};
    char temp_password[9]={};
    char (*chainhead)[9] = new char[N][9];
    
    unsigned *chaintail = new unsigned[N], sha1_input[5],hashnum, step_judge;
    
    int flag = 0;
    
    for (int i = 0; i < N; i++) {
        cin >> chainhead[i];
        getchar();
        cin >> temp_password;
        getchar();
        
        chaintail[i] = s2ui(temp_password);
    }
    
    Hash_Sha1(sha1_input);
    
      //主循环
    for (int k = 1; k <= 100; k++) {
      
        R(sha1_input, password, k);
        hashnum = s2ui(password);
        step_judge = hashnum;
      
          //找当前的p明文 hash再进行r函数以后是否和某一个彩虹链的链尾相等
        for (int s = 0; s < N; s++) {
            if (hashnum == chaintail[s]) {
                flag = Iterate(step_judge, chainhead[s]);
                if (flag) {
                    if (!flag) {
                        cout << "None\n";
                    }
                    return 0;
                }
            }
        }
        
          //往后hash然后r知道所给的R函数的i参数为100为止
        for (int j = k + 1; j <= 100; j++) {
            hashnum = Hash(password, j);
            for (int s = 0; s < N; s++) {
                if (hashnum == chaintail[s]) {
                    flag = Iterate(step_judge, chainhead[s]);
                    if (flag) {
                        if (!flag) {
                            cout << "None\n";
                        }
                        return 0;
                    }
                }
            }
        }
        
          //剩余的999次r从1到100的循环寻找碰撞
        for (int i = 1; i <= 1000; i++) {
            for (int j = 1; j <= 100; j++) {
                hashnum = Hash(password, j);
                for (int q = 0; q < N; q++) {
                    if (hashnum == chaintail[q]) {
                        flag = Iterate(step_judge, chainhead[q]);
                        if (flag) {
                            if (!flag) {
                                cout << "None\n";
                            }
                            return 0;
                        }
                    }
                }
            }
        }
          //endof主循环
    }
    
    return 0;
}
