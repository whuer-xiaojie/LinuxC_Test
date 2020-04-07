// AES_test.cpp : Defines the entry point for the console application.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include "polarssl/aes.h"
#include "mbedtls/aes.h"

//FIXME  编译的时候 需要安装openssl环境，引用libcrypto
//	gcc -o test test_aes.c -lcrypto

//测试加解密字符串 16字节一组
int test_encrypt()
{
	printf("enter test_encrypt\n");

	//密钥
	AES_KEY aes_key;

	//密钥数值
	unsigned char userKey[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00 };

	//明文空间
	//unsigned char plain[16] = "helloworld";
	unsigned char plain[1024] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4 };
	//解密后明文的空间
	//unsigned char dec_plain[16]={0};
	unsigned char dec_plain[1024] = { 0 };

	//密文空间
	//unsigned char cipher[16]={0};
	unsigned char cipher[1024] = { 0 };

	int i = 0;
	//
	time_t nowTime;
	nowTime = time(NULL);

	struct tm *sysTime = localtime(&nowTime);

	printf("系统时间：%d年%d月%d日%d时%d分\n", sysTime->tm_year + 1900, sysTime->tm_mon + 1, sysTime->tm_mday, sysTime->tm_hour, sysTime->tm_min);

	//system("pause");
	//

	//设置加密用的密钥 userKey:密钥数值 aes_key:AES_KEY对象指针
	if (AES_set_encrypt_key(userKey, 128, &aes_key) < 0)
	{
		printf("设置密钥失败!!\n");
		return 0;
	}

	printf("\n*********** 加密前：%s\n", plain);
	//加密 in, out, key:AES密钥
	//AES_ecb_encrypt(plain, cipher, &aes_key, AES_ENCRYPT);
	for (i = 0; i < 64; i++)
	{
		AES_ecb_encrypt(plain + (i * 16), cipher + (i * 16), &aes_key, AES_ENCRYPT);
	}



	printf("\n*********** 加密后：%s\n", cipher);

	//设置解密密钥 userKey:密钥数值 aes_key:AES_KEY对象指针
	if (AES_set_decrypt_key(userKey, 128, &aes_key) < 0)
	{
		printf("设置解密密钥失败!!\n");
		return 0;
	}

	//解密 in, out, key:AES密钥 
	//AES_ecb_encrypt(cipher,dec_plain,&aes_key,AES_DECRYPT);
	for (i = 0; i < 64; i++)
	{
		AES_ecb_encrypt(cipher + (i * 16), dec_plain + (i * 16), &aes_key, AES_DECRYPT);
	}

	printf("\n*********** 解密后：%s\n", dec_plain);

	printf("exit test_encrypt\n");

	return 0;
}

int main(int argc, char *argv[])
{
	test_encrypt();
	sleep(5);
	return 0;
}

// mbedTLS_test_res.cpp : Defines the entry point for the console application.
//
/*
int main(int argc, char *argv[])
{
	mbedtls_aes_context aes_ctx;
	//密钥数值
	unsigned char key[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00 };

	//明文空间
	unsigned char plain[16] = "helloworld";
	//解密后明文的空间
	unsigned char dec_plain[16] = { 0 };
	//密文空间
	unsigned char cipher[16] = { 0 };

	mbedtls_aes_init(&aes_ctx);


	//设置加密密钥
	mbedtls_aes_setkey_enc(&aes_ctx, key, 128);

	printf("\n*********** 加密前：%s\n", plain);
	mbedtls_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_ENCRYPT, plain, cipher);
	printf("\n*********** 加密后：%s\n", cipher);
	//设置解密密钥
	mbedtls_aes_setkey_dec(&aes_ctx, key, 128);

	mbedtls_aes_crypt_ecb(&aes_ctx, MBEDTLS_AES_DECRYPT, cipher, dec_plain);
	printf("\n*********** 解密后：%s\n", dec_plain);

	mbedtls_aes_free(&aes_ctx);

}
*/
/*
#define AES_BLOCK_SIZE 1024
void aes_Encrypt(char *pIn)
{
	char key[17];
	int err = 0;
	int n;
	aes_context aes;
	strcpy(key, "Hje12VCzXiu4keyy");
	unsigned char inBuf[1024] = { 0 };
	int en_len = 0;	int len;
	char arrEncodeData[1024] = { 0 };//加密后得到的数据
	char *pOutAesEncode = arrEncodeData;
	len = strlen((char*)pIn);
	printf("inputBuf:%s\n", pIn);
	aes_setkey_enc(&aes, key, 128);//  set encrypt key            
	printf("inputBuf:%s\n", pIn);
	//循环加密，这样写在公司服务器端可以解密，但是我觉得这
	//并不合规，因为如果被加密的长度是16的整数倍，其实是需要
	//扩充的，需要扩充16长度，内容为全0，然后在加密一次，这样
	//才比较标准,我们暂时不管这个
	while (en_len < len) {
		aes_crypt_ecb(&aes, AES_ENCRYPT, pIn, pOutAesEncode);
		pIn += AES_BLOCK_SIZE;
		pOutAesEncode += AES_BLOCK_SIZE;
		en_len += AES_BLOCK_SIZE;
	}
	printf("inputBuf:%s\n", pIn);
	printf("outbuf:%s\n", arrEncodeData);
}

int main(int argc, char *argv[])
{
	char *pIn = "test 123214443534";
	aes_Encrypt(pIn);
	sleep(5);
	return 0;
}
*/