//
//  main.c
//  SPN线性分析
//
//  Created by 蒋🌲齐 on 2020/9/10.
//  Copyright © 2020 蒋🌲齐. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//由于scanf速度过慢，自己改写用于优化(接受明密文对)
int scan(unsigned short * Cipertext , unsigned short * Plaintext) {
    char c = getchar();
    unsigned temp = 0;
    while( c != '\n') {
        if (c != ' ') {
            temp = temp << 4;
            if (c <= '9' && c >= '0') temp += (0 + c - '0');
            else temp += c - 'a' + 10;
        }
        else {
            * Cipertext = temp;
            temp = 0;
        }
        c = getchar();
    }
    * Plaintext = temp;
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
unsigned short SPN (unsigned short Plaintext, unsigned int key) {
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

//数组内寻找最大值,就是个排序
void MaxK2K4(unsigned short count[16][16], unsigned (*key)[8]) {
    unsigned max1 = 0, max2 = 0;
    int i, j, max = 0;
    for (i = 0; i < 16; i++)
    {
        for (j = 0; j < 16; j++)
        {
            if (abs((int)count[i][j] - 4000) > max)
            {
                max = abs((int)count[i][j] - 4000);
                max1 = (unsigned)i;
                max2 = (unsigned)j;
            }
        }
    }
    (*key)[0] = max2;
    (*key)[2] = max1;
}
void MaxK1K3(unsigned short count[16][16], unsigned (*key)[8]) {
    unsigned max1 = 0, max2 = 0;
    int i, j, max = 0;
    for (i = 0; i < 16; i++)
    {
        for (j = 0; j < 16; j++)
        {
            if (abs((int)count[i][j] - 4000) > max)
            {
                max = abs((int)count[i][j] - 4000);
                max1 = (unsigned)i;
                max2 = (unsigned)j;
            }
        }
    }
    (*key)[1] = max2;
    (*key)[3] = max1;
}

//利用教材上给出的链进行线性分析并获得K2K4
unsigned LinearChain_First(unsigned short Plaintext[8000], unsigned short Cipertext[8000], unsigned short (*count1)[16][16], unsigned (*key)[8]) {
    unsigned short K2, K4;//最后一轮，从左往右数第2和4个密钥
    unsigned short x5, x7, x8, u6, u8, u14, u16;//对应的二进制位
    unsigned short Y2, Y4;//取出的密文单元
    unsigned short temp;
    unsigned short value;//随机变量的取值
    memset((*count1), 0, sizeof(unsigned short) * 256);
    for (int i = 0; i < 8000; i++) {
        //取出当前轮要用的二进制位
        x5 = (Plaintext[i] >> 11) & 0x0001;
        x7 = (Plaintext[i] >> 9 ) & 0x0001;
        x8 = (Plaintext[i] >> 8 ) & 0x0001;
        Y2 = (Cipertext[i] >> 8 ) & 0x000F;
        Y4 =  Cipertext[i]        & 0x000F;
        
        for (K2 = 0; K2 <= 15; K2++)
        {
            for (K4 = 0; K4 <= 15; K4++)
            {
                temp = SBoxDe(K2 ^ Y2);
                u6 = (temp >> 2) & 0x0001;
                u8 = temp & 0x0001;
                temp = SBoxDe(K4 ^ Y4);
                u14 = (temp >> 2) & 0x0001;
                u16 = temp & 0x0001;
                
                //书上给的算法
                value = x5 ^ x7 ^ x8 ^ u6 ^ u8 ^ u14 ^ u16;
                
                //计数（为0则+1）
                (*count1)[K2][K4] += (1-value);
            }
        }
    }
    MaxK2K4((*count1), key);
    return 0;
}

//第二条链的查找，因为调用了第一条链分析出的K2，故新增一个参数用于分析
unsigned LinearChain_Second(unsigned short Plaintext[8000], unsigned short Cipertext[8000], unsigned short (*count2)[16][16], unsigned (*key)[8]) {
    unsigned short K1, K3;//最后一轮，从左往右数第2和4个密钥
    unsigned short x5, x6, x7, x8, u2, u4, u6, u8, u10, u12;//对应的二进制位
    unsigned short Y1, Y2, Y3;//取出的密文单元
    unsigned short temp;
    unsigned short value;//随机变量的取值
    memset((*count2), 0, sizeof(unsigned short) * 256);
    for (int i = 0; i < 8000; i++) {
        //取出当前轮要用的二进制位
        x5 = (Plaintext[i] >> 11) & 0x0001;
        x6 = (Plaintext[i] >> 10) & 0x0001;
        x7 = (Plaintext[i] >> 9 ) & 0x0001;
        x8 = (Plaintext[i] >> 8 ) & 0x0001;
        Y1 = (Cipertext[i] >> 4 ) & 0x000F;
        Y2 = (Cipertext[i] >> 8 ) & 0x000F;
        Y3 = (Cipertext[i] >> 12) & 0x000F;
        
        for (K3 = 0; K3 <= 15; K3++)
        {
            for (K1 = 0; K1 <= 15; K1++)
            {
                temp = SBoxDe(Y3 ^ K3);
                u2 = (temp >> 2) & 0x0001;
                u4 = temp & 0x0001;
                temp = SBoxDe(Y2 ^ ((*key)[2]));
                u6 = (temp >> 2) & 0x0001;
                u8 = temp & 0x0001;
                temp = SBoxDe(Y1 ^ K1);
                u10 = (temp >> 2) & 0x0001;
                u12 = temp & 0x0001;
                
                //群里偷的算法
                value = x5 ^ x6 ^ x7 ^ x8 ^ u2 ^ u4 ^ u6 ^ u8 ^ u10 ^ u12;
                
                //计数
                (*count2)[K3][K1] += (1 - value);
            }
        }
    }
    MaxK1K3((*count2), key);
    return 0;
}

unsigned KeySearch(unsigned short Plaintext[], unsigned short Ciphertext[], unsigned (*key)[8]) {
    unsigned K5K6K7K8, TempKey;
    int flag[2];
    
    for (K5K6K7K8 = 0; K5K6K7K8 <= 65535; K5K6K7K8++) {
        //创建可能的完整密钥
        TempKey = (K5K6K7K8 << 16) | ((*key)[3] << 12) | ((*key)[2] << 8) | ((*key)[1] << 4) | (*key)[0];
        
        for (int i = 0; i < 2; i++) {
            
            flag[i] = 0;
            
            if (Ciphertext[i] == SPN(Plaintext[i], TempKey)) {
                flag[i] = 1;
            }
            else
                break;
        }
        if (flag[0] & flag[1]) {
                (*key)[7] = (K5K6K7K8 >> 12) & 0x000F;
                (*key)[6] = (K5K6K7K8 >> 8 ) & 0x000F;
                (*key)[5] = (K5K6K7K8 >> 4 ) & 0x000F;
                (*key)[4] =  K5K6K7K8        & 0x000F;
                break;
        }
    }
    return flag[0] & flag[1];
}

int main () {
    
    int n;
    //明密文对数组
    unsigned short Plaintext[8000] = {0};
    unsigned short Cipertext[8000] = {0};
    //两个计数器
    unsigned short count1[16][16] = {0};
    unsigned short count2[16][16] = {0};
    //密钥中的单元
    unsigned key[8];
    int flag = 0;
    
    scanf("%d", &n);
    getchar();
    
    for (int i = 0; i < n; i++) {
        
        //接受8000对数据
        for (int j = 0; j < 8000; j++) {
            scan(&Plaintext[j], &Cipertext[j]);
        }
        
        LinearChain_First(Plaintext, Cipertext, &count1, &key);
        LinearChain_Second(Plaintext, Cipertext, &count2, &key);
        
        while (flag != 1) {
            flag = KeySearch(Plaintext, Cipertext, &key);//找到了合适密钥则不会返回0
            if (flag == 1) {
                break;
            }
            count1[key[2]][key[0]] = 4000;//将当前1链的计数器减去4000取绝对值最大的记录去除，坐标即为找到的K2K4中的两部分
            MaxK2K4(count1, &key);
            LinearChain_Second(Plaintext, Cipertext, &count2, &key);//由于1链改变重新在2链中搜索第一和第三密钥
        }
        
        //输出结果
        printf("%x%x%x%x%x%x%x%x\n", key[7], key[6], key[5], key[4], key[3], key[2], key[1], key[0]);
        flag = 0;
    }//主循环
    return 0;
}
