#include "hekr_protocol.h"

//*************************************************************************
//Hekr ¾ßÌåÂëÖµ
//*************************************************************************

//Í¨ÓÃÖ¡Í·¸ñÊ½ 
typedef struct
{
	unsigned char header;
	unsigned char length;
	unsigned char type;
	unsigned char number;
}GeneralHeader;


//Hekr¸÷Ö¡³¤¶È
typedef	enum
{
	ModuleQueryFrameLength = 0x07,
	ModuleResponseFrameLength = 0x0B,
	ErrorFrameLength = 0x07
}AllFrameLength;

//Hekr¸÷Ö¡ÀàĞÍ
typedef	enum
{
	DeviceUploadType = 0x01,
	ModuleDownloadType = 0x02,
	ModuleOperationType = 0xFE,
	ErrorFrameType = 0xFF
}AllFrameType;


//Hekr´íÎóÂëÈ¡Öµ
typedef	enum
{
	ErrorOperation = 0x01,
	ErrorSumCheck = 0x02,
	ErrorDataRange = 0x03,
	ErrorNoCMD = 0xFF
}AllErrorValue;



//Ä£¿é²éÑ¯Ö¡¸ñÊ½
typedef struct
{
	//Í¨ÓÃÖ¡Í·
	GeneralHeader header;
	//ÓĞĞ§Êı¾İ
	unsigned char CMD;
	unsigned char Reserved;
	//ºÍĞ£ÑéŒ
	unsigned char SUM;
}ModuleQueryFrame; 


//´íÎóÖ¡¸ñÊ½
typedef struct
{
	//Í¨ÓÃÖ¡Í·
	GeneralHeader header;
	//ÓĞĞ§Êı¾İ
	unsigned char ErrorCode;
	unsigned char Reserved;
	//ºÍĞ£ÑéŒ
	unsigned char SUM;
}ErrorFrame; 

//*************************************************************************
//Hekr ¶¨Òå±äÁ¿
//*************************************************************************

// ÄÚ²¿Êı¾İ
static unsigned char DATA_AREA hekr_send_buffer[USER_MAX_LEN+HEKR_DATA_LEN] = {0};
static unsigned char module_status[10] = {0};
static unsigned char frame_no = 0;
static void (*hekr_send_btye)(unsigned char);

// Ìá¹©ÓÃ»§Ê¹ÓÃ
ModuleStatusFrame  *ModuleStatus = (ModuleStatusFrame*)&module_status;
unsigned char DATA_AREA valid_data[USER_MAX_LEN] = {0};

//*************************************************************************
//Hekr ÄÚ²¿º¯ÊıÉêÃ÷  
//*************************************************************************

// Static Function
static void HekrSendByte(unsigned char ch);
static void HekrSendFrame(unsigned char DATA_AREA *dat);
static unsigned char SumCheckIsErr(unsigned char DATA_AREA *dat);
static void ErrResponse(unsigned char dat);
static unsigned char SumCalculate(unsigned char DATA_AREA *dat);
static void HekrValidDataCopy(unsigned char DATA_AREA *dat);
static void HekrModuleStateCopy(unsigned char DATA_AREA *dat);

//*************************************************************************
//Hekr º¯Êı¶¨Òå
//*************************************************************************

// ÓÃ»§º¯Êı
void HekrInit(void (*fun)(unsigned char))//Ö¸Õëº¯ÊıÖ¸ÏòÏò´®¿Ú·¢ËÍÊı¾İº¯Êı
{	
	hekr_send_btye = fun;
}

unsigned char HekrRecvDataHandle(unsigned char DATA_AREA *dat)
{
	//¼ì²éºÍĞ£Ñé
	if(SumCheckIsErr(dat))
	{
		ErrResponse(ErrorSumCheck);
		return RecvDataSumCheckErr;
	}
	//È·ÈÏÖ¡ÀàĞÍ
	switch(dat[2])
	{
	case DeviceUploadType://MCUÉÏ´«ĞÅÏ¢·´À¡ ²»ĞèÒª´¦Àí 
	                        return MCU_UploadACK;
	case ModuleDownloadType://WIFIÏÂ´«ĞÅÏ¢
	                        HekrSendFrame(dat);
	                        HekrValidDataCopy(dat);
	                        return ValidDataUpdate;
	case ModuleOperationType://HekrÄ£¿é×´Ì¬
													if(dat[1] != ModuleResponseFrameLength)
														return MCU_ControlModuleACK;
	                        HekrModuleStateCopy(dat);
	                        return HekrModuleStateUpdate;
	case ErrorFrameType://ÉÏÒ»Ö¡·¢ËÍ´íÎó	
	                        return LastFrameSendErr;
	default:ErrResponse(ErrorNoCMD);break;
	}
	return RecvDataUseless;
}

void HekrValidDataUpload(unsigned char len)
{
	unsigned char i;
	hekr_send_buffer[0] = HEKR_FRAME_HEADER;
	hekr_send_buffer[1] = len + 5;
	hekr_send_buffer[2] = DeviceUploadType;
	hekr_send_buffer[3] = frame_no++;
	for(i = 0; i < len ; i++)
		hekr_send_buffer[i+4] = valid_data[i];
	HekrSendFrame(&hekr_send_buffer[0]);
}

void HekrModuleControl(unsigned char dat)
{
	hekr_send_buffer[0] = HEKR_FRAME_HEADER;
	hekr_send_buffer[1] = ModuleQueryFrameLength;
	hekr_send_buffer[2] = ModuleOperationType;
	hekr_send_buffer[3] = frame_no++;
	hekr_send_buffer[4] = dat;
	hekr_send_buffer[5] = 0x00;
	HekrSendFrame(hekr_send_buffer);
}


// ÄÚ²¿º¯Êı
static void HekrSendByte(unsigned char ch)
{
	hekr_send_btye(ch);
}


static void HekrSendFrame(unsigned char DATA_AREA *dat)
{
	unsigned char len = dat[1];
	unsigned char i = 0;
	dat[len-1] = SumCalculate(dat);
	for(i = 0 ; i < len ; i++)
	{
		HekrSendByte(dat[i]);
	}
}

static unsigned char SumCheckIsErr(unsigned char DATA_AREA *dat)
{
	unsigned char temp = SumCalculate(dat);
	unsigned char len = dat[1] - 1;

	if(temp == dat[len])
		return 0;
	return 1;
}

static unsigned char SumCalculate(unsigned char DATA_AREA *dat)
{
	unsigned char temp;
	unsigned char i;
	unsigned char len = dat[1] - 1;

	for(temp = 0,i = 0;i < len; i++)
	{
		temp += dat[i];
	}

	return temp;
}

static void ErrResponse(unsigned char dat)
{
	hekr_send_buffer[0] = HEKR_FRAME_HEADER;
	hekr_send_buffer[1] = ErrorFrameLength;
	hekr_send_buffer[2] = ErrorFrameType;
	hekr_send_buffer[3] = frame_no++;
	hekr_send_buffer[4] = dat;
	hekr_send_buffer[5] = 0x00;
	HekrSendFrame(hekr_send_buffer);
}

static void HekrValidDataCopy(unsigned char DATA_AREA *dat)
{
	unsigned char len,i;
	len = dat[1]- HEKR_DATA_LEN;
	for(i = 0 ;i < len ; i++)
		valid_data[i] = dat[i+4];
}

static void HekrModuleStateCopy(unsigned char DATA_AREA *dat)
{
	unsigned char len,i;
	len = dat[1]- HEKR_DATA_LEN;
	for(i = 0 ;i < len ; i++)
		module_status[i] = dat[i+4];
}


