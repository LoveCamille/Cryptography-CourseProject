//
//  main.c
//  中国剩余定理
//
//  Created by 蒋🌲齐 on 2020/10/10.
//  Copyright © 2020 蒋🌲齐. All rights reserved.
//

#include <stdio.h>
#include <gmp.h>

mpz_t ret;          //用作返回值

//扩展欧拉，偷来的
void Mod_Inverse(mpz_t E, mpz_t N) {
    mpz_t X1, X2, X3, Y1, Y2, Y3, T1, T2, T3, Q, P, e, n;
    
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
    mpz_init(e);
    mpz_init(n);
    
    mpz_set(e, E);
    mpz_set(n, N);
    
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

int main() {
    mpz_t p, q, e, d, key, FN, N, flag;
    
    mpz_init(ret);
    //初始化
    mpz_init(p);
    mpz_init(q);
    mpz_init(e);
    mpz_init(d);
    mpz_init(key);
    mpz_init(FN);
    mpz_init(N);
    mpz_init(flag);
    mpz_set_ui(flag, 1);
    
    int n;
    scanf("%d", &n);
    
    gmp_scanf("%Zd %Zd %Zd", &p, &q, &e);
    
    mpz_mul(N, p, q);
    mpz_sub_ui(p, p, 1);
    mpz_sub_ui(q, q, 1);
    mpz_mul(FN, p, q);
    mpz_add_ui(p, p, 1);
    mpz_add_ui(q, q, 1);
    
    Mod_Inverse(e, FN);
    mpz_set(d, ret);

    while (n--) {
        mpz_t c, sum1, sum2, sum3, P, Q;
        
        mpz_init(c);
        mpz_init(sum1);
        mpz_init(sum2);
        mpz_init(sum3);
        mpz_init(P);
        mpz_init(Q);
        
        mpz_set_ui(sum2, 1);
        mpz_set_ui(sum3, 1);
        
        gmp_scanf("%Zd", &c);
        mpz_powm(sum2, c, d, p);
        mpz_powm(sum3, c, d, q);
        
        Mod_Inverse(q, p);
        
        mpz_set(Q, ret);
        mpz_mul(sum2, sum2, q);
        mpz_mul(sum2, sum2, Q);
        
        Mod_Inverse(p, q);
        
        mpz_set(P, ret);
        mpz_mul(sum3, sum3, p);
        mpz_mul(sum3, sum3, P);
        
        mpz_add(sum1, sum2, sum3);
        mpz_fdiv_r(sum1, sum1, N);
        gmp_printf("%Zd\n", sum1);
    }
    return 0;
}
