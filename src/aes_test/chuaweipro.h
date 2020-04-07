/*
* huawei_protocol.cpp
*
*  Created on: 2018/9/4
*      Author: the second group of Sansi Software nstitute
*/

#ifndef CHUAWEI_PROTOCOL_H
#define CHUAWEI_PROTOCOL_H

#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "../polarssl/aes.h"


#define AES256_ENCRYPT_IV_LEN   16
#define AES_KEY_SIZE            32
#define buff_size               40960

#define EEM_HEART_BEAT          0x00
#define EEM_DEVICE_SELFCHECK    0x05
#define EEM_QUERY_DATA          0x11
#define EEM_PENETRATE           0x76
#define EEM_PENETRATE_REPORT    0x77
#define EEM_ENCRYPTION          0x66

#define EEM_FRAME_END           0x16

#define EEM_FRAME_HEAD_LEN      0x04
#define EEM_FRAME_COMID_LEN     0x04
#define EEM_FRAME_EFFLEN_OFFSET 0x04
#define EEM_FRAME_ORDER_OFFSET  0x06
#define EEM_FRAME_DATA_OFFSET   0x07

#define HUAWEI_ID               0x89
using namespace std;

class CHuaweiPro
{
public:
	CHuaweiPro(char *pu8Key,bool aesFlag=false);
	~CHuaweiPro();

	int32_t ExecHuaweiOrder(char *pOrder, int32_t orderLen, 
		char *pResultData, int32_t &resultLen);

	int32_t GetPenetrateData(char *pSrcOrder, int32_t srcLen, char *pData, int32_t &len);
	int32_t BuildPenetrateData(char *pData, int32_t &len);
	int32_t BuildSelfCheckData(char *pData, int32_t &len);
	int32_t BuildHeartBeatData(char *pData, int32_t &len);
	int32_t BuildSensorQueryData(char *pData, int32_t &len);

private:
	//int socket_fd;
	bool m_aesFlag;
	string m_pu8Key;
	unsigned char m_IV[AES256_ENCRYPT_IV_LEN];
	aes_context m_aesContext;

private:
	unsigned char SumCrcCheck(const char *buf, size_t count);
	void CalculateAesIV(void);
	void BuildFrameHead(char *pData);
	void BuildFrameCommunicationId(char *pData);
	void BuildEncrypedFrame(char *pOrder, int32_t &orderLen, unsigned char *pPacket,
		int32_t packetLen,int32_t encrypLen);
	int32_t EncryptionOrder(char *pOrder, int32_t &orderLen);
	int32_t DecryptionOrder(char *pOrder, int32_t &orderLen);
	bool ValidFrame(const char *pOrder, int32_t orderLen);
};


#endif