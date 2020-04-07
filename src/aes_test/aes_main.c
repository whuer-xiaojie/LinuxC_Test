#include <stdio.h>
#include <stdlib.h>

#include "polarssl/aes.h"

#define DIV_ROUND_UP(n, d)	(((n) + (d) - 1) / (d))


#define  MAX_FRAME_SIZE 1024;

//#define MAX(a,b) {(a)>(b)? (a):(b)}
#define MAX(a,b)  (((a)>(b))?(a):(b))

static char pu8key[32 * 8];


void initpu8key()
{
	int i = 0;
	memset(pu8key, 0x00, 32 * 8);

	pu8key[i++] = 'H';
	pu8key[i++] = 'u';
	pu8key[i++] = 'a';
	pu8key[i++] = 'w';
	pu8key[i++] = 'e';
	pu8key[i++] = 'i';
	pu8key[i++] = '@';
	pu8key[i++] = '1';
	pu8key[i++] = '2';
	pu8key[i++] = '3';

}

void calculate_aes_iv(char *IV)
{
	int i = 0;
	memset(IV, 0, 16);
	//for (i = 0; i < 16; i++){
	//	IV[i] = (unsigned char)(((i*i * 3) % 56)*i);
	//}
	IV[0] = 1;
	IV[1] = 2;
	IV[2] = 3;
	IV[3] = 4;
	IV[4] = 5;
	IV[5] = 6;
}

void encryption_order(char *pOrder, int size)
{
	if (pOrder == NULL || size <= 0)
		return;

	int n = DIV_ROUND_UP(MAX(16, size), 16) * 16;
	unsigned char iv[16];
	calculate_aes_iv(iv);
	unsigned char packet[1024];
	int32_t packetLen = 0;

	aes_context aesContext;

	aes_init(&aesContext);

	aes_setkey_enc(&aesContext, (unsigned char *)pu8key, 32 * 8);

	if (aes_crypt_cbc(&aesContext, AES_ENCRYPT, n, iv, pOrder, packet) != 0){
		fprintf(stderr, "%s():AES Encryption failed\n", __FUNCTION__);
		return -1;
	}
	int i = 0;
	
	fprintf(stderr, "%s(): size=%d packet=", __FUNCTION__,n);
	for (i = 0; i < n; i++)
		fprintf(stderr, "%x", packet[i]);
	fprintf(stderr, "\n");
}
void decryption_order(char *pOrder, int size)
{
	if (pOrder == NULL || size <= 0)
		return;

	//unsigned char buf[MAX_FRAME_SIZE];
	//int32_t bufLen;
	unsigned char packet[1024];
	int32_t packetLen = 0;

	unsigned char iv[16];
	calculate_aes_iv(iv);

	aes_context aesContext;

	aes_init(&aesContext);
	calculate_aes_iv(iv);

	aes_setkey_dec(&aesContext, (unsigned char *)pu8key, 32 * 8);

	if (aes_crypt_cbc(&aesContext, 0, size, iv, pOrder, packet) != 0){
		fprintf(stderr, "%s():AES Decryption Failed\n", __FUNCTION__);
		return;
	}
	fprintf(stderr, "packet=%s\n", packet);
}

int main(int argc, char **argv)
{
	char *pTest = "UsPwVvzeyFhbglmpSLr9cP68zU2Uwn9QPKvl+aIhnID53cp3ZXKxE4FygJAPoRlq9z2sah4zO9fHqagXWfN8tPK+vMi1AIWpYuULdLD1EBr4rpuzGq+zpm7pFXcxbkuTKfVGTBN29q1E4M3Mw8Vvw7RiNyLHRbk68cz9xeJdEJDJEHNTn+o68mnWM9tQxbl3x2t94uJv/ke9ZVV4q1J8+sm7w6EVXH1wq+guS0Mg89uiXjugvjW/7u7XJuFkPbtAGLxI/iH5NIMW7b3O9+mGv8lcQQI+mKpRjmJ1knH0nKZchtPE61UrKIYf3dFBUr02cFzVpc9EwhtJR+miP+Y05A==";
	decryption_order(pTest, strlen(pTest));

	char *pTest2 = "123456";
	encryption_order(pTest2, strlen(pTest2));

	return 0;
}
