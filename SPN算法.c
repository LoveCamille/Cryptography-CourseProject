//
//  main.c
//  SPN算法的实现
//
//  Created by 蒋🌲齐 on 2020/9/7.
//  Copyright © 2020 蒋🌲齐. All rights reserved.
//

#include <stdio.h>

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

//s盒代换(E,4,D,1,2,F,B,8,3,A,6,C,5,9,0,7)
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

//s盒代换解密(E,3,4,8,1,C,A,F,7,D,9,6,B,2,0,5)
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

//由于scanf速度过慢，自己改写用于优化
int scan(unsigned * key , unsigned short * Plaintext)
{
    char c = getchar();
    unsigned temp = 0;
    while( c != '\n') {
        if (c != ' ') {
            temp = temp << 4;
            if (c <= '9' && c >= '0') temp += (0 + c - '0');
            else temp += c - 'a' + 10;
        }
        else {
            * key = temp;
            temp = 0;
        }
        c = getchar();

    }
    *Plaintext = temp;
    return 0;
}

int main() {
    //设置输入的组数
    int n;
    scanf("%d", &n);
    getchar();
    
    unsigned short Plaintext;
    unsigned int key;
    
    for (int i = 0; i < n; i++) {
        scan(&key, &Plaintext);
        
        //一共五轮加密

        for (int i = 1; i < 5; i++) {
            Plaintext ^= Rollkey(key, i);
            unsigned short temp;
            //代换部分
            temp =  (SBoxEn( Plaintext >> 12) << 12);
            temp += (SBoxEn((Plaintext & 0x0F00) >> 8) << 8);
            temp += (SBoxEn((Plaintext & 0x00F0) >> 4) << 4);
            temp +=  SBoxEn( Plaintext & 0x000F);
            Plaintext = temp;
            //置换部分
            Plaintext = PBoxEn(Plaintext);
        }
        //第5轮
        Plaintext = PBoxEn(Plaintext);
        Plaintext ^= Rollkey(key, 5);
        printf("%04x ", Plaintext);
        
        //一共五轮解密
        Plaintext ^= 1;
        Plaintext ^= Rollkey(key, 5);
        
        for (int i = 4; i > 0; i--) {
            unsigned short temp;
            //代换部分
            temp = (SBoxDe(Plaintext >> 12) << 12);
            temp += (SBoxDe((Plaintext & 0x0F00) >> 8) << 8);
            temp += (SBoxDe((Plaintext & 0x00F0) >> 4) << 4);
            temp += SBoxDe(Plaintext & 0x000F);
            Plaintext = temp;
            
            Plaintext ^= Rollkey(key, i);
            
            //置换部分
            Plaintext = PBoxEn(Plaintext);
        }
        Plaintext = PBoxEn(Plaintext);
        printf("%04x\n", Plaintext);
    }//主循环
    return 0;
}

