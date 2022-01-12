//
//  main.c
//  SPN差分分析
//
//  Created by 蒋🌲齐 on 2020/9/17.
//  Copyright © 2020 蒋🌲齐. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//由于scanf速度过慢，自己改写用于优化
int scan(unsigned * main) {
    char c = getchar();
    unsigned temp = 0;
    int i = 0;
    while (c != '\n') {
        if (c != ' ') {
            temp = temp << 4;
            if (c <= '9' && c >= '0') {
                temp += (0 + c - '0');
            }
            else {
                temp += c - 'a' + 10;
            }
        }
        else {
            *(main + i) = temp;
            temp = 0;
            i++;
        }
        c = getchar();
    }
    *(main + i) = temp;
    return 0;
}

//轮密钥计算函数（返回值为生成的轮密钥）
unsigned short int Rollkey(unsigned int key, int roll) {
    
    /*unsigned short rollkey;//生成的轮密钥，16位
    //左移到对应位
    key = key << (roll - 1) * 4;
    //右移消除无效bit
    rollkey = key >> 16;
    return rollkey*/
    
    return (key << (roll - 1) * 4) >> 16;
}

//s盒代换(E,4,D,1,2,F,B,8,3,A,6,C,5,9,0,7)（每次换4个2进制位/1个16进制位）
unsigned short SBoxEn(unsigned short Plaintext) {
    switch (Plaintext) {
        case 0:
            return 0xE;
        case 1:
            return 0x4;
        case 2:
            return 0xD;
        case 3:
            return 0x1;
        case 4:
            return 0x2;
        case 5:
            return 0xF;
        case 6:
            return 0xB;
        case 7:
            return 0x8;
        case 8:
            return 0x3;
        case 9:
            return 0xA;
        case 0xA:
            return 0x6;
        case 0xB:
            return 0xC;
        case 0xC:
            return 0x5;
        case 0xD:
            return 0x9;
        case 0xE:
            return 0x0;
        case 0xF:
            return 0x7;
        default:
            return 0;
    }
}

//s盒代换解密(E,3,4,8,1,C,A,F,7,D,9,6,B,2,0,5)（每次换4个2进制位/1个16进制位）
unsigned short SBoxDe(unsigned short Plaintext) {
    switch (Plaintext) {
        case 0:
            return 0xE;
        case 1:
            return 0x3;
        case 2:
            return 0x4;
        case 3:
            return 0x8;
        case 4:
            return 0x1;
        case 5:
            return 0xC;
        case 6:
            return 0xA;
        case 7:
            return 0xF;
        case 8:
            return 0x7;
        case 9:
            return 0xD;
        case 0xA:
            return 0x9;
        case 0xB:
            return 0x6;
        case 0xC:
            return 0xB;
        case 0xD:
            return 0x2;
        case 0xE:
            return 0x0;
        case 0xF:
            return 0x5;
        default:
            return 0;
    }
}

//对整个密文进行一次S盒代换
unsigned short FullSBoxEn(unsigned short Plaintext) {
    unsigned short temp;
    //代换部分
    temp = (SBoxEn(Plaintext >> 12) << 12);
    temp += (SBoxEn((Plaintext & 0x0F00) >> 8) << 8);
    temp += (SBoxEn((Plaintext & 0x00F0) >> 4) << 4);
    temp += SBoxEn(Plaintext & 0x000F);
    return temp;
}

//对整个密文进行一次S盒代换
unsigned short FullSBoxDe(unsigned short Plaintext) {
    unsigned short temp;
    //代换部分
    temp = (SBoxDe(Plaintext >> 12) << 12);
    temp += (SBoxDe((Plaintext & 0x0F00) >> 8) << 8);
    temp += (SBoxDe((Plaintext & 0x00F0) >> 4) << 4);
    temp += SBoxDe(Plaintext & 0x000F);
    return temp;
}

//p盒置换
//(1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,9 ,10,11,12,13,14,15,16)
//(1 ,5 ,9 ,13,2 ,6 ,10,14,3 ,7 ,11,15,4 ,8 ,12,16)
unsigned short PBoxEn(unsigned short Plaintext) {
    unsigned short result;
    //1->1
    result = Plaintext & 0b1000000000000000;
    //2->5
    result += ((Plaintext & 0b0100000000000000) >> 3);
    //3->9
    result += ((Plaintext & 0b0010000000000000) >> 6);
    //4->13
    result += ((Plaintext & 0b0001000000000000) >> 9);
    //5->2
    result += ((Plaintext & 0b0000100000000000) << 3);
    //6->6
    result +=  (Plaintext & 0b0000010000000000) ;
    //7->10
    result += ((Plaintext & 0b0000001000000000) >> 3);
    //8->14
    result += ((Plaintext & 0b0000000100000000) >> 6);
    //9->3
    result += ((Plaintext & 0b0000000010000000) << 6);
    //10->7
    result += ((Plaintext & 0b0000000001000000) << 3);
    //11->11
    result +=  (Plaintext & 0b0000000000100000) ;
    //12->15
    result += ((Plaintext & 0b0000000000010000) >> 3);
    //13->4
    result += ((Plaintext & 0b0000000000001000) << 9);
    //14->8
    result += ((Plaintext & 0b0000000000000100) << 6);
    //15->12
    result += ((Plaintext & 0b0000000000000010) << 3);
    //16->16
    result +=  (Plaintext & 0b0000000000000001) ;
    return result;
}

//spn加密函数
unsigned short SPN(unsigned short Plaintext, unsigned int key) {
    for (int i = 1; i < 5; i++) {
        Plaintext ^= Rollkey(key, i);
        unsigned short temp;
        //代换部分
        temp = (SBoxEn(Plaintext >> 12) << 12);
        temp += (SBoxEn((Plaintext & 0x0F00) >> 8) << 8);
        temp += (SBoxEn((Plaintext & 0x00F0) >> 4) << 4);
        temp += SBoxEn(Plaintext & 0x000F);
        Plaintext = temp;
        //置换部分
        Plaintext = PBoxEn(Plaintext);
    }
    //第5轮
    Plaintext = PBoxEn(Plaintext);
    Plaintext ^= Rollkey(key, 5);
    return Plaintext;
}

int main() {
    unsigned cip[65536];
    int n;
    scanf("%d", &n);
    getchar();
    while (n--) {
        scan(cip);
        int count1[16][16] = { 0 };
        int count2[16][16] = { 0 };
        unsigned x1, x2, y1, y2;
        int max = -1;
        int k1, k2, k3, k4;
        unsigned key_lo;
        int flag = 0;
        unsigned key1;
        int z = 0;
        for (x1 = 0; x1 < 65536; x1++)
        {
            x2 = x1 ^ 2816;
            y1 = cip[x1];
            y2 = cip[x2];
            if (!((y1^y2) & 61680))
            {
                for (int i = 0; i < 16; i++)
                {
                    for (int j = 0; j < 16; j++)
                    {
                        unsigned key = (i << 8) | j;
                        unsigned u = FullSBoxDe(y1 ^ key);
                        unsigned U = FullSBoxDe(y2^key);
                        if ((u ^ U) == 1542)
                        {
                            count1[i][j]++;
                            z++;
                            if (z == 150)
                                goto out1;
                        }
                    }
                }
            }
        }
        
    out1:
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                if (count1[i][j] > max)
                {
                    k2 = i;
                    k4 = j;
                    max = count1[i][j];
                }
            }
        }
        z = 0;

        for (x1 = 0; x1 < 65536; x1++)
        {
            x2 = x1 ^ 61680;
            y1 = cip[x1];
            y2 = cip[x2];
            if (!((y1 ^ y2) & 61455))
            {
                for (int j = 0; j < 16; j++)
                {
                    unsigned key = (k2 << 8) | (j << 4);
                    unsigned u = FullSBoxDe(y1 ^ key);
                    unsigned U = FullSBoxDe(y2^key);
                    if ((u ^ U) == 2176)
                    {
                        count2[0][j] ++;
                        z++;
                        if (z == 150)
                            goto out2;
                    }
                }
            }
        }
        
    out2:
        max = -1;
        for (int j = 0; j < 16; j++)
        {
            if (count2[0][j] > max)
            {
                k3 = j;
                max = count2[0][j];
            }
        }
        z = 0;
        memset(count2, 0, 256 * sizeof(unsigned));

        for (x1 = 0; x1 < 65536; x1++)
        {
            x2 = x1 ^ 512;
            y1 = cip[x1];
            y2 = cip[x2];
            if (!((y1^y2) & 3855))
            {
                for (int i = 0; i < 16; i++)
                {

                    unsigned key = (i << 12) | (k3 << 4);
                    unsigned u = FullSBoxDe(y1 ^ key);
                    unsigned U = FullSBoxDe(y2^key);
                    if ((u ^ U) == 24672)
                    {
                        count2[i][0] ++;
                        z++;
                        if (z == 180)
                            goto restart;
                    }
                }
            }
        }

    restart:
        max = -1;
        for (int i = 0; i < 16; i++)
        {
            if (count2[i][0] > max)
            {
                k1 = i;
                max = count2[i][0];
            }
        }
        if (max == 0)
        {
            count1[k2][k4] = 0;
            goto out1;
        }
        key_lo = (k1 << 12) | (k2 << 8) | (k3 << 4) | k4;
        flag = 0;
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 16; j++)
            {
                for (int m = 0; m < 16; m++)
                {
                    for (int n = 0; n < 16; n++)
                    {
                        key1 = (i << 12) | (j << 8) | (m << 4) | n;
                        unsigned key2 = (j << 12) | (m << 8) | (n << 4) | k1;
                        unsigned key3 = (m << 12) | (n << 8) | (k1 << 4) | k2;
                        unsigned key4 = (n << 12) | (k1 << 8) | (k2 << 4) | k3;
                        if ((PBoxEn(FullSBoxEn(PBoxEn(FullSBoxEn(PBoxEn(FullSBoxEn(61440 ^ key1)) ^ key2)) ^ key3)) ^ key4) == FullSBoxDe(cip[61440] ^ key_lo))
                        {
                            if ((PBoxEn(FullSBoxEn(PBoxEn(FullSBoxEn(PBoxEn(FullSBoxEn(512 ^ key1)) ^ key2)) ^ key3)) ^ key4) == FullSBoxDe(cip[512] ^ key_lo))
                            {
                                flag = 1;
                                goto finish;
                            }
                        }
                    }
                }
            }
        }
        
    finish:
        if (flag == 1)
        {
            printf("%08x\n", (key1 << 16) | key_lo);
        }
        else
        {
            count2[k1][0] = 0;
            goto restart;
        }
    }
}
