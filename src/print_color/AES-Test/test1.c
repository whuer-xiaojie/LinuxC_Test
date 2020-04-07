#include<stdio.h>
//#include "polarssl/aes.h"
//#include "polarssl/compat-1.2.h"
#include "mbedtls/aes.h"
#include "mbedtls/compat-1.3.h"

#define AES_ECB 0
#define AES_CBC 1
#define AES_CFB 2
#define AES_CTR 3
#define MODE AES_ECB

unsigned char key[16] = { 0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22 };
unsigned char plain[32] = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x20, 0x21, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 };
unsigned char plain_decrypt[32] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
unsigned char IV[16];
unsigned char cypher[32];
int i = 0;
mbedtls_aes_context aes;

void SetIV()
{
	int i;
	for (i = 0; i < 16; i++)
	{
		IV[i] = 0x55;
	}

}
int main()
{

	int j = 0;

	if (MODE == AES_ECB)
	{
		mbedtls_aes_setkey_enc(&aes, key, 128);//  set encrypt key			
		mbedtls_aes_crypt_ecb(&aes, AES_ENCRYPT, plain, cypher);
		mbedtls_aes_setkey_dec(&aes, key, 128);//  set decrypt key
		mbedtls_aes_crypt_ecb(&aes, AES_DECRYPT, cypher, plain_decrypt);
		i++;
		
		printf("plain: ");
		for (j = 0; j < 32; j++)
			printf("0x%02X ", plain[j]);
		printf("\n");

		printf("cypher: ");
		for (j = 0; j < 32; j++)
			printf("0x%02X ", cypher[j]);
		printf("\n");

		printf(" plain_decrypt: ");
		for (j = 0; j < 32; j++)
			printf("0x%02X ", plain_decrypt[j]);
		printf("\n");
	}
	
	//printf("plain:%s\n cypher:%s\n plain_decrypt:%s\n", plain, cypher, plain_decrypt);
	if (MODE == AES_CBC)
	{
		mbedtls_aes_setkey_enc(&aes, key, 128);//  set encrypt key
		SetIV();
		mbedtls_aes_crypt_cbc(&aes, AES_ENCRYPT, 32, IV, plain, cypher);
		mbedtls_aes_setkey_dec(&aes, key, 128);//  set decrypt key
		SetIV();
		mbedtls_aes_crypt_cbc(&aes, AES_DECRYPT, 32, IV, cypher, plain_decrypt);
		i++;
		printf("plain: ");
		for (j = 0; j < 32; j++)
			printf("0x%02X ", plain[i]);
		printf("\n");

		printf("cypher: ");
		for (j = 0; j < 32; j++)
			printf("0x%02X ", cypher[i]);
		printf("\n");

		printf(" plain_decrypt: ");
		for (j = 0; j < 32; j++)
			printf("0x%02X ", plain_decrypt[i]);
		printf("\n");
	}

	
	//printf("plain:%s\n cypher:%s\n plain_decrypt:%s\n", plain, cypher, plain_decrypt);
}
