/*
* huawei_protocol.cpp
*
*  Created on: 2018/9/4
*      Author: the second group of Sansi Software Institute
*/
#include "CHuaweiPro.h"
#include "tcp_client.h"
#include "../polarssl/aes.h"

#include <ctype.h>
#include <inttypes.h>

extern "C"{
#include "../libunix.h"
}

static const char huaweiFrameHead[] = { 0xFE, 0xFE, 0xFE, 0x68 };
static const char communicationId[] = { 0x00, 0x00, 0x89, 0x00 };

/* tcp_client.cpp */
extern int32_t tcp_debug;
extern char deviceID[8];

CHuaweiPro::CHuaweiPro(char *pu8Key, bool aesFlag)
{
	
	if (pu8Key != NULL)
		m_pu8Key = pu8Key;
	else
		m_pu8Key = "huawei@123";

	CalculateAesIV();
	aes_init(&m_aesContext);
	m_aesFlag = aesFlag;
}

CHuaweiPro::~CHuaweiPro()
{
	aes_free(&m_aesContext);
}

int32_t CHuaweiPro::ExecHuaweiOrder(char *pOrder, int32_t orderLen, char *pResultData, int32_t &resultLen)
{
	int32_t ret = -1;

	if (!ValidFrame(pOrder, orderLen))
		return ret;


	int32_t sum = SumCrcCheck((pOrder + 4), orderLen - 6);
	if (sum != pOrder[orderLen - 2])
		return ret;


	int32_t effLength = pOrder[EEM_FRAME_EFFLEN_OFFSET] | pOrder[EEM_FRAME_EFFLEN_OFFSET + 1] << 8;
	uint8_t orderType = pOrder[EEM_FRAME_ORDER_OFFSET];//&0x7F;

	if (tcp_debug == 1){
		fprintf(stderr, "%s():Execute Huawei TCP Order 0x%02X\n", __FUNCTION__, orderType);
	}

	switch (orderType){

	case EEM_HEART_BEAT:
		if (tcp_debug == 1)
			fprintf(stderr, "%s():Huawei Heart Beat Success\n", __FUNCTION__);
		ret = 0;
		break;

	case EEM_DEVICE_SELFCHECK:
		if (BuildSelfCheckData(pResultData, resultLen) == 0)
			ret = 1;
		break;

	case EEM_QUERY_DATA:
		if (BuildSensorQueryData(pResultData, resultLen) == 0)
			ret = 1;
		break;

	case EEM_PENETRATE:
		if (GetPenetrateData(pOrder, orderLen, pResultData, resultLen) == 0)
			ret = 2;
		break;

	case EEM_PENETRATE_REPORT:
		ret = 0;
		break;

	case EEM_ENCRYPTION:
		if (DecryptionOrder(pOrder, orderLen) == -1){
			return ret;
		}
		ret = ExecHuaweiOrder(pOrder, orderLen, pResultData, resultLen);
		break;

	default:
		break;
	}

	return ret;
}

/*******************************************************************/
int32_t CHuaweiPro::GetPenetrateData(char *pSrcOrder, int32_t srcLen, char *pData, int32_t &len)
{
	if (NULL == pSrcOrder || srcLen < 13 || NULL == pData)
		return -1;
	len = 0;
	//pOrder=4+2+1+4+1+parameter(2+1+n)+data+sum+end
	//head+effective length+type+communication id+AES flag+parameter+data+sum+end
	for (int i = 11; i < srcLen - 13; i++)
		pData[len++] = pSrcOrder[i];
	return 0;
}

int32_t CHuaweiPro::BuildPenetrateData(char *pData, int32_t &len)
{
	if (NULL == pData)
		return -1;

	char buf[MAX_FRAME_SIZE];
	int32_t bufLen = 0;

	BuildFrameHead(buf);
	bufLen += EEM_FRAME_HEAD_LEN + 2;

	//build the order type and data
	buf[bufLen++] = EEM_PENETRATE | 0x80;
	BuildFrameCommunicationId(buf + bufLen);
	bufLen += EEM_FRAME_COMID_LEN;
	buf[bufLen++] = 0xFF;//chn id

	for (int i = 0; i < len; i++)
		buf[len++] = pData[i];

	buf[4] = (bufLen - 6) & 0xFF;
	buf[5] = ((bufLen - 6) >> 8) & 0xFF;

	buf[bufLen++] = SumCrcCheck(buf, bufLen);
	buf[bufLen++] = EEM_FRAME_END;

	if (m_aesFlag){
		EncryptionOrder(buf, bufLen);
	}

	for (int i = 0; i < bufLen; i++)
		pData[i] = buf[i];

	len = bufLen;
	return len;
}

int32_t CHuaweiPro::BuildSelfCheckData(char *pData, int32_t &len)
{
	if (NULL == pData)
		return -1;

	len = 0;

	BuildFrameHead(pData);
	len += EEM_FRAME_HEAD_LEN + 2;

	//build the order type and data 
	pData[len++] = EEM_DEVICE_SELFCHECK | 0x80;
	BuildFrameCommunicationId(pData + len);
	len += EEM_FRAME_COMID_LEN;
	
	for (int i = 0; i < DEVICEID_LEN; i++)
		pData[len++] = deviceID[i];
	
	pData[len++] = 0x79;
	pData[len++] = 0x01;
	pData[len++] = 0x0a;
	pData[len++] = 0x29;

	pData[len++] = 0x00;
	pData[len++] = 0x00;

	pData[len++] = 0x79;
	pData[len++] = 0x01;
	pData[len++] = 0x0a;
	pData[len++] = 0x29;

	pData[4] = (len - 6) & 0xFF;
	pData[5] = ((len - 6) >> 8) & 0xFF;
	
	pData[len++] = SumCrcCheck(pData, len);
	pData[len++] = EEM_FRAME_END;

	if (m_aesFlag == true){	
		return EncryptionOrder(pData, len);
	}
	
	return 0;
}

int32_t CHuaweiPro::BuildHeartBeatData(char *pData, int32_t &len)
{
	if (NULL == pData)
		return -1;

	len = 0;

	BuildFrameHead(pData);
	len += EEM_FRAME_HEAD_LEN + 2;

	//build the order type and data 
	pData[len++] = EEM_HEART_BEAT | 0x80;
	BuildFrameCommunicationId(pData + len);
	len += EEM_FRAME_COMID_LEN;

	pData[4] = (len - 6) & 0xFF;
	pData[5] = ((len - 6) >> 8) & 0xFF;

	pData[len++] = SumCrcCheck(pData, len);
	pData[len++] = EEM_FRAME_END;

	if (m_aesFlag){
		return EncryptionOrder(pData, len);
	}

	return 0;
}

int32_t CHuaweiPro::BuildSensorQueryData(char *pData, int32_t &len)
{

}

/*******************************************************************/
void CHuaweiPro::CalculateAesIV(void)
{
	for (int i = 0; i < AES256_ENCRYPT_IV_LEN; i++)
		m_IV[i] = (uint8_t)(((i*i * 3) % 56)*i);
}

bool CHuaweiPro::ValidFrame(const char *pOrder, int32_t orderLen)
{
	if (NULL == pOrder || orderLen <= 5)
		return false;

	//check head
	for (int i = 0; i < 4; i++){
		if (pOrder[i] != huaweiFrameHead[i])
			return false;
	}

	if (pOrder[orderLen - 1] != EEM_FRAME_END)
		return false;

	return true;
}

unsigned char CHuaweiPro::SumCrcCheck(const char *buf, size_t count)
{
	unsigned char checksum = 0;
	for (size_t i = 4; i < count; i++)
		checksum += (unsigned char)buf[i];
	return checksum;
}

void CHuaweiPro::BuildFrameHead(char *pData)
{
	for (int i = 0; i < EEM_FRAME_HEAD_LEN; i++)
		pData[i] = huaweiFrameHead[i];
}

void CHuaweiPro::BuildFrameCommunicationId(char *pData)
{
	for (int i = 0; i < EEM_FRAME_COMID_LEN; i++)
		pData[i] = communicationId[i];
}

void CHuaweiPro::BuildEncrypedFrame(char *pOrder, int32_t &orderLen, unsigned char *pPacket,
	int32_t packetLen, int32_t encrypLen)
{
	if (NULL == pOrder || NULL == pPacket)
		return;
	orderLen = 0;

	//build Huawei frame head
	BuildFrameHead(pOrder);
	orderLen += EEM_FRAME_HEAD_LEN + 2;

	//frame type
	pOrder[orderLen++] = 0x66;

	//communication id
	BuildFrameCommunicationId(pOrder + orderLen);
	orderLen += EEM_FRAME_COMID_LEN;

	//aes256 encryption flag
	pOrder[orderLen++] = 1;

	//encryption data length
	pOrder[orderLen++] = encrypLen & 0xFF;
	pOrder[orderLen++] = (encrypLen >> 8) & 0xFF;

	//aes256 iv info
	pOrder[orderLen++] = AES256_ENCRYPT_IV_LEN;
	for (int i = 0; i < AES256_ENCRYPT_IV_LEN; i++)
		pOrder[orderLen++] = m_IV[i];

	//encrypted data
	for (int i = 0; i < packetLen; i++)
		pOrder[orderLen++] = pPacket[i];

	pOrder[4] = (orderLen - 6) & 0xFF;//effective length low byte
	pOrder[5] = ((orderLen - 6) >> 8) & 0xFF;//effective length height byte

	pOrder[orderLen++] = SumCrcCheck(pOrder, orderLen);
	pOrder[orderLen++] = EEM_FRAME_END;
}

int32_t CHuaweiPro::EncryptionOrder(char *pOrder, int32_t &orderLen)
{
	if (NULL == pOrder || orderLen < 8)
		return -1;

	if (tcp_debug == 1)
		fprintf(stderr, "%s():***************len=%d\n", __FUNCTION__, orderLen);

	unsigned char packet[MAX_FRAME_SIZE];
	unsigned char send_buf[MAX_FRAME_SIZE];
	int32_t send_len = 0;
	
	for (int i = EEM_FRAME_ORDER_OFFSET; i < orderLen - 8; i++){
		send_buf[send_len++] = pOrder[i];
	}

	size_t n = DIV_ROUND_UP(MAX(16, send_len), AES256_ENCRYPT_IV_LEN) * AES256_ENCRYPT_IV_LEN;

	aes_setkey_enc(&m_aesContext, (unsigned char *)m_pu8Key.data(), AES_KEY_SIZE * 8);
	
	if (aes_crypt_cbc(&m_aesContext, AES_ENCRYPT, n, m_IV, send_buf, packet) != 0){
		fprintf(stderr, "%s():AES Encryption failed\n", __FUNCTION__);
		return -1;
	}
	
	BuildEncrypedFrame(pOrder, orderLen, packet, n, send_len);
	
	return 0;
}

int32_t CHuaweiPro::DecryptionOrder(char *pOrder, int32_t &orderLen)
{
	if (NULL == pOrder)
		return -1;
	unsigned char buf[MAX_FRAME_SIZE];
	int32_t bufLen;
	unsigned char packet[MAX_FRAME_SIZE];
	int32_t packetLen = 0;

	unsigned char iv[16];

	int8_t ivLen = pOrder[14];
	int8_t encryptType = pOrder[11];

	packetLen = pOrder[13] | (pOrder[14] << 8);

	for (int i = 0; i < ivLen; i++){
		iv[i] = pOrder[15 + i];
	}

	memcpy(buf, pOrder + 15 + ivLen, orderLen - 17 - ivLen);
	bufLen = orderLen - 17 - ivLen;

	aes_setkey_dec(&m_aesContext, (unsigned char *)m_pu8Key.data(), AES_KEY_SIZE * 8);
	if (aes_crypt_cbc(&m_aesContext, AES_DECRYPT, bufLen, iv, buf, packet) != 0){
		fprintf(stderr, "%s():AES Decryption Failed\n", __FUNCTION__);
		return -1;
	}

	BuildFrameHead(pOrder);
	orderLen = EEM_FRAME_HEAD_LEN;

	pOrder[orderLen++] = packetLen & 0xFF;
	pOrder[orderLen++] = (packetLen >> 8) & 0xFF;

	for (int i = 0; i < packetLen; i++){
		pOrder[orderLen++] = packet[i];
	}

	buf[orderLen++] = SumCrcCheck(pOrder, orderLen);
	buf[orderLen++] = EEM_FRAME_END;

	return orderLen;
}