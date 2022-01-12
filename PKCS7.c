//
//  main.c
//  PKCS7
//
//  Created by 蒋🌲齐 on 2020/10/10.
//  Copyright © 2020 蒋🌲齐. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/sm3.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/sm2.h>
#include <openssl/pkcs7.h>

const char cacert[] = "\
-----BEGIN CERTIFICATE----- \n\
MIIB/zCCAaagAwIBAgIJAKKa0PAt9M1FMAoGCCqBHM9VAYN1MFsxCzAJBgNVBAYT \n\
AkNOMQ4wDAYDVQQIDAVIdUJlaTEOMAwGA1UEBwwFV3VIYW4xDTALBgNVBAoMBEhV \n\
U1QxDDAKBgNVBAsMA0NTRTEPMA0GA1UEAwwGY2Fyb290MB4XDTIwMDkyMDIwNTkx \n\
OVoXDTMwMDkxODIwNTkxOVowWzELMAkGA1UEBhMCQ04xDjAMBgNVBAgMBUh1QmVp \n\
MQ4wDAYDVQQHDAVXdUhhbjENMAsGA1UECgwESFVTVDEMMAoGA1UECwwDQ1NFMQ8w \n\
DQYDVQQDDAZjYXJvb3QwWTATBgcqhkjOPQIBBggqgRzPVQGCLQNCAASJ8mm28JJR \n\
bZKLr6DCo1+KWimpKEsiTfZM19Zi5ao7Au6YLosyN71256MWmjwkwXxJeLa0lCfm \n\
kF/YWCX6qGQ0o1MwUTAdBgNVHQ4EFgQUAL5hW3RUzqvsiTzIc1gUHeK5uzQwHwYD \n\
VR0jBBgwFoAUAL5hW3RUzqvsiTzIc1gUHeK5uzQwDwYDVR0TAQH/BAUwAwEB/zAK \n\
BggqgRzPVQGDdQNHADBEAiAaZMmvE5zzXHx/TBgdUhjtpRH3Jpd6OZ+SOAfMtKxD \n\
LAIgdKq/v2Jkmn37Y9U8FHYDfFqk5I0qlQOAmuvbVUi3yvM= \n\
-----END CERTIFICATE----- \n\
";

const char pkeyB[] = "\
-----BEGIN EC PARAMETERS----- \n\
BggqgRzPVQGCLQ== \n\
-----END EC PARAMETERS----- \n\
-----BEGIN EC PRIVATE KEY----- \n\
MHcCAQEEINQhCKslrI3tKt6cK4Kxkor/LBvM8PSv699Xea7kTXTToAoGCCqBHM9V \n\
AYItoUQDQgAEH7rLLiFASe3SWSsGbxFUtfPY//pXqLvgM6ROyiYhLkPxEulwrTe8 \n\
kv5R8/NA7kSSvcsGIQ9EPWhr6HnCULpklw== \n\
-----END EC PRIVATE KEY----- \n\
";

char out[1000];//作为输出

X509 * getX509(const char* cert) {
    BIO* bio;
    //生成一个mem型的BIO结构
    bio = BIO_new(BIO_s_mem());
    //往BIO中写入一个以NULL为结束符的字符串，成功就返回真正写入的数据的长度，失败返回0或－1
    BIO_puts(bio, cert);
    return  PEM_read_bio_X509(bio, NULL, NULL, NULL);
}

EVP_PKEY* getpkey(const char* private_key) {
    BIO* bio_pkey = BIO_new_mem_buf((char*)private_key, strlen(private_key));
    if (bio_pkey == NULL) {
        return NULL;
    }
    return  PEM_read_bio_PrivateKey(bio_pkey, NULL, NULL, NULL);//读取私钥
}

int x509cb(int ok, X509_STORE_CTX* ctx) {
    return ok;
}

int main() {
    int flag = 0;
    
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    
    BIO* en_bio = NULL;//用于存储加密的信息
    PKCS7* p7 = NULL;
    en_bio = BIO_new_fd(fileno(stdin), BIO_NOCLOSE);//从stdin读入数据
    p7 = PEM_read_bio_PKCS7(en_bio, NULL, NULL, NULL);//转化为pkcs7类型数据
    
    if (p7 == NULL) {
        printf("ERROR\n");
        return 0;
    }
    
    
    X509_STORE* rootCertStore = X509_STORE_new();//创建x509证书-ca证书
    X509_STORE_CTX* rootCertStoreCTX = X509_STORE_CTX_new();//创建证书存储区上下文环境函数
    X509_STORE_add_cert(rootCertStore, getX509(cacert));//添加证书
    X509_STORE_set_verify_cb_func(rootCertStore, x509cb);//自验证
    BIO* de_bio = NULL;//用于存储解密信息
    de_bio = PKCS7_dataDecode(p7, getpkey(pkeyB), NULL, NULL);//获取私钥，解密消息
    int length = BIO_read(de_bio, out, sizeof(out));//读取消息长度
    
    
    if (length > 0) {
        out[length] = 0;
    }
    else {
        printf("ERROR\n");
        return 0;
    }
    
    
    STACK_OF(PKCS7_SIGNER_INFO)* stack = NULL;//用于存储签名信息
    PKCS7_SIGNER_INFO* signInfo;
    stack = PKCS7_get_signer_info(p7);//获得签名者信息
    
    
    if (stack != NULL) {
        int num = sk_PKCS7_SIGNER_INFO_num(stack);//获取签名者个数
        for (int i = 0; i < num; i++) {
            signInfo = sk_PKCS7_SIGNER_INFO_value(stack, i);//依次获得签名者的信息
            int num2 = PKCS7_dataVerify(rootCertStore, rootCertStoreCTX, de_bio, p7, signInfo);
            if (num2 <= 0) {
                printf("ERROR\n");
                return 0;
            }
        }
    }
    else {
        printf("ERROR\n");
        return 0;
    }
    
    
    PKCS7_free(p7);
    BIO_free(en_bio);
    BIO_free(de_bio);
    
    printf("%s", out);
    return 0;
}
