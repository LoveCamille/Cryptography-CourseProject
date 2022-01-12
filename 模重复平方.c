//
//  main.c
//  模重复平方
//
//  Created by 蒋🌲齐 on 2020/10/6.
//  Copyright © 2020 蒋🌲齐. All rights reserved.
//

#include <stdio.h>
#include <gmp.h>

int main() {
    
    int n;
    scanf("%d", &n);
    
    while (n--) {
        
        mpz_t e, m, p, q, N;
        
        //初始化
        mpz_init(e);
        mpz_init(m);
        mpz_init(p);
        mpz_init(q);
        mpz_init(N);
        
        gmp_scanf("%Zd%Zd%Zd%Zd", &e, &m, &p, &q);
        
        mpz_mul(N, p, q);
        mpz_t sum, key, tag;//sum为总和，a为中间量
        
        //初始化
        mpz_init(sum);
        mpz_init(key);
        mpz_init(tag);
      
        unsigned long k = 0;//用来记录位数
        
        mpz_set_ui(sum, 1);
        mpz_set_ui(tag, 1);
        
        while (mpz_cmp_ui(e, 0) != 0) {
            
            mpz_and(key, e ,tag);
            
            if (mpz_cmp_ui(key, 1) == 0) {
                mpz_mul(sum, sum, m);
                //求余数
                mpz_fdiv_r(sum, sum, N);
            }
            
            mpz_mul(m, m, m);
            mpz_fdiv_r(m, m, N);
            mpz_fdiv_q_ui(e, e, 2);
        }
        
        //输出 结果
        gmp_printf("%Zd\n", sum);
        
    }
    return 0;
}
