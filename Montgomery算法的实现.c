//
//  main.c
//  Montgomery算法的实现
//
//  Created by 蒋🌲齐 on 2020/10/10.
//  Copyright © 2020 蒋🌲齐. All rights reserved.
//

#include <cstring>
#include <gmp.h>

char s1[100010],s2[100010],s3[100010];

mpz_t a,b,c;
mpz_t ret,tmp,mod;
size_t k;

int kd = 8 * sizeof(mp_limb_t); // kd 为 montgomery 算法一个单元的位数

// 求逆元
int cnt;
mpz_t st[10010];

void Inverse(mpz_t inv,const mpz_t x,const mpz_t m){
    mpz_t a,b;
    mpz_init_set(a,m);
    mpz_init_set(b,x);

    cnt=0;
    
    while(mpz_cmp_ui(b,0)){
        mpz_fdiv_q(tmp,a,b);
        mpz_init_set(st[cnt],tmp);cnt++;

        mpz_mod(tmp,a,b);
        mpz_set(a,b);
        mpz_set(b,tmp);
    }
    
    mpz_init_set_ui(a,0);
    mpz_init_set_ui(b,1);
    for(int i = cnt-2;~i;i--){
        mpz_submul(a,st[i],b);
        mpz_mod(a,a,m);
        mpz_set(tmp,a);
        mpz_set(a,b);
        mpz_set(b,tmp);
    }
    mpz_set(inv,b);
    mpz_clear(a);
    mpz_clear(b);
}

// 这个函数使用底层函数 mpn，常数比较小，使用方面有诸多限制
// 计算 a * b，'*' 为 montgemery 乘法，保存在 t 中，a 和 b 均为 montgomery 数
mpz_t d,dmask,p,q,e,key,ip,iq,kp,kq,r[35],irp[35],irq[35],imp,imq,t0,T;
void Montgomery(mpz_t x,const mpz_t a,const mpz_t b,const mpz_t m,const mp_limb_t im){
    k = m->_mp_size;
    mp_limb_t *head,num,in,res[64]={0},t[64]={0};

    // mpn_mul 要求第一个乘数的limbs数大于第二个
    if(a->_mp_size>b->_mp_size)
       mpn_mul(t,a->_mp_d,a->_mp_size,b->_mp_d,b->_mp_size);
    else
       mpn_mul(t,b->_mp_d,b->_mp_size,a->_mp_d,a->_mp_size);

    head=t;
    for(int i=0;i<k;i++){
         num=head[0]*im; // num = t[0] * im
        res[i]=mpn_addmul_1(head,m->_mp_d,k,num);
        // t = t + m * num
        // 加法只处理 0 ~ k-1 的 k 个 limbs，第 k 号 limbs 保存在res[i]中
        head++;//向右移 kd 位 相当于整除2^32
    }

    in=mpn_add_n(head,head,res,k);//将进位res[i]一次性地加到t上
    if (in!=0||mpn_cmp(head,m->_mp_d,k)>= 0)//判断是否需要减 m
        mpn_sub_n(head,head,m->_mp_d, k);

    mpz_import(x,k,-1,sizeof(mp_limb_t),0,0,head);//将得到的结果保存在x中
}

//从一班同学那里偷来的理解，可以实现快速的猛哥🐎力
// 使用了 montgomery 优化的滑动窗口快速幂
mpz_t g[77]; // 根据数据规模，窗口位数为 6 时最快
void QuickPow_SlideWin(mpz_t x,const mpz_t a,const mpz_t b,const mpz_t m,const mpz_t im,const mpz_t ir) {
    mpz_set(ret,r[k]);
    mpz_mod(ret,ret,m);
    mpz_mul(g[1],a,r[k]);
    mpz_mod(g[1],g[1],m);

    mpz_get_str(s1,2,b);

    for(int i=2;i<64;i++) {
        Montgomery(g[i],g[1],g[i-1],m,*(im->_mp_d));
    }
    
    for(int i=0,j,e,l=strlen(s1); i<l; ) {
        j=i+6<l?i+6:l;e=0;
        for(int o = i; o < j; o++){
            e<<=1;
            e|=s1[o]-'0';
            Montgomery(ret,ret,ret,m,*(im->_mp_d));
        }
        Montgomery(ret,ret,g[e],m,*(im->_mp_d));

        i=j;while(s1[i]=='0'&&i<l){i++;Montgomery(ret,ret,ret,m,*(im->_mp_d));}
    }
    mpz_mul(ret,ret,ir);
    mpz_mod(ret,ret,m);
    mpz_set(x,ret);
    return 0;
}

int main() {
    int n;
    scanf("%d",&n);

    mpz_inits(e,p,q,a,b,c,T,t0,NULL);
    mpz_inits(ip,iq,imp,imq,ret,NULL);
    
    for(int i=0;i<64;i++)mpz_init(g[i]);

    mpz_init_set_ui(d,2);
    mpz_pow_ui(d,d,kd); // d = 2 ^ 32
    mpz_init_set(dmask,d);
    mpz_sub_ui(dmask,dmask,1); // dmask = 0x ff ff ff ff

    gmp_scanf("%Zd%Zd%Zd",p,q,e);

    for(int i=1;i<=32;i++){
        mpz_init_set(r[i],d);
        mpz_pow_ui(r[i],r[i],i); // r = 2 ^ (32 * i)
        Inverse(irp[i],r[i],p);
        Inverse(irq[i],r[i],q);
    }

    //    这里的预处理和第 7 题一样
    mpz_init_set_ui(mod,1);
    mpz_init_set(key,e);

    mpz_set(tmp,p);
    mpz_sub_ui(tmp,tmp,1);
    mpz_mul(mod,mod,tmp);

    mpz_set(tmp,q);
    mpz_sub_ui(tmp,tmp,1);
    mpz_mul(mod,mod,tmp); // mod 为 (p - 1) * (q - 1)

    Inverse(key,key,mod); // key 为 e ^ (-1) (mod mod)
    mpz_mul(mod,p,q); // 再将 mod 设置为 p * q

    Inverse(ip,p,q);mpz_mul(ip,p,ip);mpz_mod(ip,ip,mod); // ip = p ^ (-1) (mod mod)
    Inverse(iq,q,p);mpz_mul(iq,q,iq);mpz_mod(iq,iq,mod); // iq = q ^ (-1) (mod mod)

    mpz_init_set(kp,p);mpz_sub_ui(kp,kp,1);mpz_mod(kp,key,kp); // kp = key (mod p - 1)
    mpz_init_set(kq,q);mpz_sub_ui(kq,kq,1);mpz_mod(kq,key,kq); // kq = key (mod q - 1)

    mpz_invert(imp,p,d);mpz_sub(imp,d,imp);// imp= -inv(p) (mod 2 ^ 32 = d)
    mpz_invert(imq,q,d);mpz_sub(imq,d,imq);// imq= -inv(q) (mod 2 ^ 32 = d)


    while(n--){
        gmp_scanf("%Zd",c);

        k = p->_mp_size;
        QuickPow_SlideWin(a,c,kp,p,imp,irp[k]);
        mpz_mul(a,a,iq);
        mpz_mod(a,a,mod);

        k=q->_mp_size;
        QuickPow_SlideWin(b,c,kq,q,imq,irq[k]);
        mpz_mul(b,b,ip);
        mpz_mod(b,b,mod);

        mpz_add(ret,a,b); 
        mpz_mod(ret,ret,mod);

        gmp_printf("%Zd\n",ret);
    }
    return 0;
}
