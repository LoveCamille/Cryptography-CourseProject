//
//  main.c
//  SPN增强
//
//  Created by 蒋🌲齐 on 2020/9/24.
//  Copyright © 2020 蒋🌲齐. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KEYBYTES (16) //密钥字节长度
#define INPUTBYTES (1<<24) //明文字节长度
#define SPNBYTES (8) //SPN加密长度，需要改动

//参照课本写DES

//以下为所需的8个S盒
unsigned long S1[4][16] = {
    {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
    {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
    {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
    {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}
};

unsigned long S2[4][16] = {
    {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
    {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
    {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
    {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}
};

unsigned long S3[4][16] = {
    {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
    {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
    {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
    {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}
};

unsigned long S4[4][16] = {
    {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
    {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
    {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
    {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}
};

unsigned long S5[4][16] = {
    {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
    {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
    {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
    {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}
};

unsigned long S6[4][16] = {
    {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
    {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
    {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
    {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}
};

unsigned long S7[4][16] = {
    {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
    {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
    {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
    {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}
};

unsigned long S8[4][16] = {
    {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
    {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
    {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
    {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
};

unsigned long KP[56] = {
    57, 49, 41, 33, 25, 17, 9 , 1 , 58, 50, 42, 34, 26, 18,
    10, 2 , 59, 51, 43, 35, 27, 19, 11, 3 , 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15, 7 , 62, 54, 46, 38, 30, 22,
    14, 6 , 61, 53, 45, 37, 29, 21, 13, 5 , 28, 20, 12, 4
};//密钥从64位到56位的置换表

unsigned long CP[48] = {
    14, 17, 11, 24, 1 , 5 , 3 , 28, 15, 6 , 21, 10,
    23, 19, 12, 4 , 26, 8 , 16, 7 , 27, 20, 13, 2 ,
    41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};//密钥从56位压缩置换到48位

unsigned long KEY[2];
unsigned long Rollkey[17];

unsigned long Extend(unsigned long temp)//将32位扩展到48位
{
    unsigned long a32,a1,z1,z2,z3,z4,z5,z6,z7,z8;
    a32=temp&1;//取第32个比特位
    a1=(temp>>31)&1;//取第一个比特位
    temp=(temp<<1)|(a32<<33)|a1;//temp为34位，a32-a1-a2...a32-a1
    z1=(temp>>28)&0x3f;
    z2=(temp>>24)&0x3f;
    z3=(temp>>20)&0x3f;
    z4=(temp>>16)&0x3f;
    z5=(temp>>12)&0x3f;
    z6=(temp>>8)&0x3f;
    z7=(temp>>4)&0x3f;
    z8=temp&0x3f;
    temp=(z1<<42)|(z2<<36)|(z3<<30)|(z4<<24)|(z5<<18)|(z6<<12)|(z7<<6)|z8;
    return temp;
}

void KeyRoll(unsigned long key)//密钥编排，将64位密钥置变成两个48位的固定子密钥
{
    int i;
    unsigned long result=0,L=0,R=0,key1=0,key2=0;
    for(i=1; i <= 56; i++) {//先经过Kp置换为56位
        result=result|((key>>(64-KP[i-1]))&1)<<(56-i);
    }
    L=(result>>28)&0xfffffff;//取左28位
    R=result&0xfffffff ;//取右28位
    key1=((((L<<1)|(L>>27))&0xfffffff)<<28)|(((L<<1)|(L>>27))&0xfffffff);//左右两部分循环左移1位
    key2=((((L<<2)|(L>>26))&0xfffffff)<<28)|(((L<<2)|(L>>26))&0xfffffff);//左右两部分循环左移2位
    KEY[0]=0;
    KEY[1]=0;
    for(i=1;i<=48;++i) {
        //经过压缩置换将56位置换位48位
        KEY[0]=KEY[0]|(((key1>>(48-CP[i-1]))&1)<<(48-i));
        KEY[1]=KEY[1]|(((key2>>(48-CP[i-1]))&1)<<(48-i));
    }
}

//P盒置换函数
unsigned long PBoxEn(unsigned long v) {
    /*unsigned long temp = 0;
    temp |= ((v>>16)&1)<<31;
    temp |= ((v>>25)&1)<<30;
    temp |= ((v>>12)&1)<<29;
    temp |= ((v>>11)&1)<<28;
    temp |= ((v>>3 )&1)<<27;
    temp |= ((v>>20)&1)<<26;
    temp |= ((v>>4 )&1)<<25;
    temp |= ((v>>15)&1)<<24;
    temp |= ((v>>31)&1)<<23;
    temp |= ((v>>17)&1)<<22;
    temp |= ((v>>9 )&1)<<21;
    temp |= ((v>>6 )&1)<<20;
    temp |= ((v>>27)&1)<<19;
    temp |= ((v>>14)&1)<<18;
    temp |= ((v>>1 )&1)<<17;
    temp |= ((v>>22)&1)<<16;
    temp |= ((v>>30)&1)<<15;
    temp |= ((v>>24)&1)<<14;
    temp |= ((v>>8 )&1)<<13;
    temp |= ((v>>18)&1)<<12;
    temp |= ((v>>0 )&1)<<11;
    temp |= ((v>>5 )&1)<<10;
    temp |= ((v>>29)&1)<<9 ;
    temp |= ((v>>23)&1)<<8 ;
    temp |= ((v>>13)&1)<<7 ;
    temp |= ((v>>19)&1)<<6 ;
    temp |= ((v>>2 )&1)<<5 ;
    temp |= ((v>>26)&1)<<4 ;
    temp |= ((v>>10)&1)<<3 ;
    temp |= ((v>>21)&1)<<2 ;
    temp |= ((v>>28)&1)<<1 ;*/
    
    return (((v>>16)&1)<<31)|(((v>>25)&1)<<30)|(((v>>12)&1)<<29)|(((v>>11)&1)<<28)|(((v>>3)&1)<<27)|(((v>>20)&1)<<26)|(((v>>4)&1)<<25)|(((v>>15)&1)<<24)|(((v>>31)&1)<<23)|(((v>>17)&1)<<22)|(((v>>9)&1)<<21)|(((v>>6)&1)<<20)|(((v>>27)&1)<<19)|(((v>>14)&1)<<18)|(((v>>1)&1)<<17)|(((v>>22)&1)<<16)|(((v>>30)&1)<<15)|(((v>>24)&1)<<14)|(((v>>8)&1)<<13)|(((v>>18)&1)<<12)|(((v>>0)&1)<<11)|(((v>>5)&1)<<10)|(((v>>29)&1)<<9)|(((v>>23)&1)<<8)|(((v>>13)&1)<<7)|(((v>>19)&1)<<6)|(((v>>2)&1)<<5)|(((v>>26)&1)<<4)|(((v>>10)&1)<<3)|(((v>>21)&1)<<2)|(((v>>28)&1)<<1)|(((v>>7)&1)<<0);
}

unsigned long f(unsigned long A,unsigned long J) {//A为32比特，J为48比特 ，完成输入的右半部分和轮密钥的f函数
    
    unsigned long EA= Extend(A);//将明文的右半部分从32位扩展到48位
    unsigned long result0 = EA ^ J;//将扩展后的明文右半部分和轮密钥异或
    
    unsigned long B1 = (result0 >> 42) & 0x3F;//6位分为一组，共8组
    unsigned long B2 = (result0 >> 36) & 0x3F;
    unsigned long B3 = (result0 >> 30) & 0x3F;
    unsigned long B4 = (result0 >> 24) & 0x3F;
    unsigned long B5 = (result0 >> 18) & 0x3F;
    unsigned long B6 = (result0 >> 12) & 0x3F;
    unsigned long B7 = (result0 >> 6)  & 0x3F;
    unsigned long B8 =  result0      & 0x3F;
    
    //S盒部分
    B1 = S1[((B1>>4)&2)|(B1&1)][(B1>>1)&15];
    B2 = S2[((B2>>4)&2)|(B2&1)][(B2>>1)&15];
    B3 = S3[((B3>>4)&2)|(B3&1)][(B3>>1)&15];
    B4 = S4[((B4>>4)&2)|(B4&1)][(B4>>1)&15];
    B5 = S5[((B5>>4)&2)|(B5&1)][(B5>>1)&15];
    B6 = S6[((B6>>4)&2)|(B6&1)][(B6>>1)&15];
    B7 = S7[((B7>>4)&2)|(B7&1)][(B7>>1)&15];
    B8 = S8[((B8>>4)&2)|(B8&1)][(B8>>1)&15];
    
    unsigned long C = (B1 << 28) | (B2 << 24) | (B3 << 20) | (B4 << 16) | (B5 << 12) | (B6 << 8) | (B7 << 4) | B8;//C为经过s盒的32位
    
    return PBoxEn(C);
}

//SPN加密函数
void SPN(unsigned long x,unsigned long * y) {
    unsigned long L, R, temp, result, w=x;//w位输入的明文
    int i,Nr=16;
    L = (w >> 32) & 0xffffffff;//左半部分
    R = w & 0xffffffff;//右半部分
    for(i = 1; i <= Nr; i++) {//进行16轮加密
        temp = R;
        result = f(R,Rollkey[i]);
        R = L ^ result;
        L = temp;
    }
    *y = (L << 32) | R;//将经过16轮加密后的结果写入密文指针
}

int main() {
    
    unsigned long key[2], Plaintext=0, Ciphertext=0;
    
    int i;
    
    for(i = 1; i <= 16; i++) {//roundkey中保存16个轮密钥
        Rollkey[i] = KEY[1];
    }
    
    Rollkey[1] = KEY[0];
    Rollkey[2] = KEY[0];
    Rollkey[9] = KEY[0];
    Rollkey[16]= KEY[0];
    
    fread(key, KEYBYTES, 1, stdin);//读入128位密钥
    
    KeyRoll(key[0]);//KEY数组中保存固定的两个子密钥
    
    for(i=0; i < (INPUTBYTES / SPNBYTES); i++) {
        
        fread(&Plaintext, 8, 1, stdin);//读入明文
        
        Plaintext = Plaintext ^ Ciphertext;
        
        SPN(Plaintext, &Ciphertext);
        SPN(Ciphertext, &Ciphertext);
        SPN(Ciphertext, &Ciphertext);
        
        fwrite(&Ciphertext,SPNBYTES,1,stdout);//输出密文
    }
    return 0;
}
