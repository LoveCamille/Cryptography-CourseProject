//
//  main.c
//  RSA算法的实现
//
//  Created by 蒋🌲齐 on 2020/9/3.
//  Copyright © 2020 蒋🌲齐. All rights reserved.
//

#include <stdio.h>
#include <gmp.h>

mpz_t ret;//用作返回值

void Mod_Inverse(mpz_t e, mpz_t n) {
    mpz_t X1, X2, X3, Y1, Y2, Y3, T1, T2, T3, Q, P;
    
    mpz_init(X1);
    mpz_init(X2);
    mpz_init(X3);
    mpz_init(Y1);
    mpz_init(Y2);
    mpz_init(Y3);
    mpz_init(T1);
    mpz_init(T2);
    mpz_init(T3);
    mpz_init(Q);
    mpz_init(P);
    
    if (mpz_cmp(e, n) > 0) {
        mpz_mod(e, e, n);
    }
    
    mpz_set_ui(X1, 1);
    mpz_set_ui(X2, 0);
    mpz_set(X3, n);
    mpz_set_ui(Y1, 0);
    mpz_set_ui(Y2, 1);
    mpz_set(Y3, e);
    
    for(;;) {
        if (mpz_cmp_ui(Y3, 0) == 0) {
            break;
        }
        if (mpz_cmp_ui(Y3, 1) == 0) {
            break;
        }
        mpz_fdiv_q(Q, X3, Y3);//X3=Y3*Q+r
        mpz_mul(P, Q, Y1);//(T1,T2,T3)<-(X1-Q*Y1,X2-Q*Y2,X3-Q*Y3)
        mpz_sub(T1, X1, P);
        mpz_mul(P, Q, Y2);
        mpz_sub(T2, X2, P);
        mpz_mul(P, Q, Y3);
        mpz_sub(T3, X3, P);
        mpz_set(X1, Y1);//(X1,X2,X3)<-(Y1,Y2,Y3)
        mpz_set(X2, Y2);
        mpz_set(X3, Y3);
        mpz_set(Y1, T1);//(Y1,Y2,Y3)<-(T1,T2,T3)
        mpz_set(Y2, T2);
        mpz_set(Y3, T3);
    }
    
    if (mpz_cmp_ui(Y2, 0) < 0) {
        mpz_add(ret, Y2, n);
    }
    else
        mpz_set(ret, Y2);
}

void Get_Gcd(mpz_t c, mpz_t d) {
    mpz_t r, x, y;
    
    mpz_init(r);
    mpz_init(x);
    mpz_init(y);
    
    mpz_set(x, c);
    mpz_set(y, d);
    
    
    mpz_fdiv_r(r, x, y);
    
    //辗转相除
    while (mpz_cmp_ui(r, 0) != 0) {
        mpz_set(x, y);
        mpz_set(y, r);
        mpz_fdiv_r(r, x, y);
    }
    
    mpz_set(ret, y);
    mpz_clear(r);
}

int main() {
    
    mpz_t  V_n, p, q, e, p_1, q_1;
    
    //初始化
    mpz_init(V_n);
    mpz_init(p);
    mpz_init(q);
    mpz_init(e);
    mpz_init(p_1);
    mpz_init(q_1);
    
    mpz_init(ret);
    
    //循环
    int n;
    scanf("%d", &n);
    
    for (int i = 0; i < n; i++) {
        gmp_scanf("%Zd%Zd%Zd", &e, &p, &q);
        if ((mpz_cmp_ui(e, 65536) <= 0)) {//e太小
            printf("ERROR\n");
            continue;
        }
        if ((mpz_probab_prime_p(p, 35) == 0) || (mpz_probab_prime_p(q, 35) == 0)) {//p,q不是素数
            printf("ERROR\n");
            continue;
        }
        mpz_mul(ret, p, q);//p,q间隔太小
        mpz_sqrt(ret, ret);
        
        mpz_t retn;
        
        mpz_init(retn);
        
        mpz_sub(retn, p, ret);
        mpz_abs(retn, retn);
        
        if (mpz_cmp_ui(retn, (1 << 31)) < 0) {
            printf("ERROR\n");
            continue;
        }
        mpz_sub(retn, ret, q);
        mpz_abs(retn, retn);
        if (mpz_cmp_ui(retn, (1 << 31)) < 0) {
            printf("ERROR\n");
            continue;
        }
        mpz_sub_ui(p_1, p, 1);//p=p-1
        mpz_sub_ui(q_1, q, 1);//q=q-1
        mpz_mul(V_n, p_1, q_1);//V_n=(p-1)*(q-1)
        
        Get_Gcd(q_1, p_1);
        //p-1,q-1过于平滑
        if (mpz_cmp_ui(ret, 16) > 0) {
            printf("ERROR\n");
            continue;
        }
        
        Get_Gcd(e, V_n);
        if (mpz_cmp_ui(ret, 1) != 0) {
            printf("ERROR\n");
            continue;
        }
        
        Mod_Inverse(e, V_n);
        gmp_printf("%Zd\n", ret);
    }
    return 0;
}




