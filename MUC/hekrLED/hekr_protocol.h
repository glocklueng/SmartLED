#ifndef	_HEKR_PROTOCOL_H_
#define	_HEKR_PROTOCOL_H_

// HEKR USER API **************************************************************

//Ê¹ÓÃÇ°ÒªÈ·¶¨ÓÃ»§ËùĞèÒªµÄ×î´óÊı×é  Ä¬ÈÏÎª100 ´óÓÚ100ĞèÒª×ÔĞĞĞŞ¸Ä
//Êı×é´óĞ¡¿ÉÒÔ×ÔĞĞĞŞ¸ÄÎª×î³¤³¤¶È  
//Èç¹ûÓĞ¶àÌõ²»µÈ³¤ÃüÁî  È¡×î³¤³¤¶È  ÎªÓÃ»§Êı¾İ³¤¶È  ·ÇÕûÖ¡³¤¶È
//#define USER_MAX_LEN 0x64u

//Hekr Ğ­Òé³õÊ¼»¯
//Ê¹ÓÃHekrĞ­ÒéÇ°ĞèÍê³É³õÊ¼»¯
//³õÊ¼»¯ĞèÒªÓÃ»§ÓĞ´®¿Ú·¢ËÍÒ»¸öbyteµÄ³ÌĞò
//eg:  void UART_SendChar(u8 ch); ´«Êä²ÎÊı±ØĞëÖ»ÊÇÒ»¸ö8bitµÄÊı
//     ¸Ãº¯ÊıĞèÒªÓÃ»§×ÔĞĞÔÚ³ÌĞòÖĞ¶¨Òå
//HekrInitº¯Êı:
//´«Èë²ÎÊıÎªÓÃ»§´®¿Ú·¢ËÍÒ»¸öbyteº¯ÊıµÄº¯ÊıÃû
//void HekrInit(void (*fun)(unsigned char));
//eg:  HekrInit(UART_SendChar);   

//´«Èë´®¿Ú½ÓÊÕµÄÊı¾İÊı×é  
//·µ»ØÖµ¼ûÍ·ÎÄ¼ş RecvDataHandleCode
//´«ÈëÊı×é³¤¶ÈÓ¦´óÓÚÓÃ»§Êı¾İ³¤¶È¼ÓÉÏHEKR_DATA_LEN
//Êı¾İ±£´æÔÚ¶ÔÓ¦Êı×éÖĞ valid_data ºÍ ModuleStatus Ö¸Õë
//unsigned char HekrRecvDataHandle(unsigned char* data);

//ÅäÖÃ¼°²éÑ¯hekrÄ£¿é×´Ì¬ ´«ÈëÂëÖµ¼ûÍ·ÎÄ¼ş HekrModuleControlCode
//×´Ì¬Öµ±£´æÔÚmodule_statusÊı×éÖĞ
//void HekrModuleControl(unsigned char data);


//ÉÏ´«ÓÃ»§ÓĞĞ§Êı¾İ
//Êı¾İ´æ·ÅÔÚvalid_dataÊı×éÖĞ len ÎªÓÃ»§Êı¾İ³¤¶È  ·ÇÕûÖ¡³¤¶È
//void HekrValidDataUpload(unsigned char len);

//Ğ­ÒéĞŞ¸ÄÈÕÆÚ 2015.09.01 
//Ğ­ÒéÍøÖ·  http://docs.hekr.me/protocol/
//BUG ·´À¡  pengyu.zhang@hekr.me
//					965006619@qq.com
//*****************************************************************************




#define USER_MAX_LEN 0x64u
#define HEKR_DATA_LEN 0x05u
#define HEKR_FRAME_HEADER 0x48u //Èç¹ûÊı¾İÊÇ0x48 ÎŞ·ûºÅÀàĞÍ


#define _KEIL_51_SDK_

#ifdef	_KEIL_51_SDK_
#define	DATA_AREA		xdata
#else
#define	DATA_AREA		
#endif 

//*************************************************************************
//
//ModuleStatus Ö¸Õë °üº¬ÄÚÈİ®¹
//
//*************************************************************************

//Ä£¿éÓ¦´ğÖ¡¸ñÊ½
typedef struct
{
//ÓĞĞ§Êı¾İ
	unsigned char CMD;
	unsigned char Mode;
	unsigned char WIFI_Status;
	unsigned char CloudStatus;
	unsigned char SignalStrength;// 0-5 ´ú±íĞÅºÅÇ¿¶È
	unsigned char Reserved;
}ModuleStatusFrame; 


//*************************************************************************
//
//HekrRecvDataHandle  º¯Êı·µ»ØÖµ
//
//*************************************************************************

typedef	enum
{
	RecvDataSumCheckErr = 0x01,
	LastFrameSendErr = 0x02,
	MCU_UploadACK = 0x03,
	ValidDataUpdate = 0x04,
	RecvDataUseless = 0x05,
	HekrModuleStateUpdate = 0x06,
	MCU_ControlModuleACK = 0x07
}RecvDataHandleCode;


//HekrÄ£¿é¿ØÖÆÂë
typedef	enum
{
	ModuleQuery = 0x01,
	ModuleRestart = 0x02,
	ModuleRecover = 0x03,
	HekrConfig = 0x04
}HekrModuleControlCode;


//*************************************************************************
//
//ModuleStatus Ö¸ÕëÖĞ¸÷¸öÓĞĞ§Î»¾ßÌåÂëÖµ
//
//*************************************************************************

//HekrÄ£¿é×´Ì¬Âë
typedef	enum
{
	STA_Mode = 0x01,
	HekrConfig_Mode = 0x02,
	AP_Mode = 0x03,
	STA_AP_Mode = 0x04,
	RF_OFF_Mode = 0x05
}HekrModuleWorkCode;

//Hekr WIFI×´Ì¬Âë
typedef	enum
{
	RouterConnected = 0x01,
	RouterConnectedFail = 0x02,
	RouterConnecting = 0x03,
	PasswordErr = 0x04,
	NoRouter = 0x05,
	RouterTimeOver = 0x06
}HekrModuleWIFICode;

//Hekr Cloud×´Ì¬Âë
typedef	enum
{
	CloudConnected = 0x01,
	DNS_Fail = 0x02,
	CloudTimeOver = 0x03
}HekrModuleCloudCode;


//*************************************************************************
//ÓÃ»§Êı¾İÇøº
//*************************************************************************

extern unsigned char valid_data[USER_MAX_LEN];
extern ModuleStatusFrame *ModuleStatus; 


//*************************************************************************
//º¯ÊıÁĞ±í
//*************************************************************************

// Hekr USER API 
void HekrInit(void (*fun)(unsigned char));
unsigned char HekrRecvDataHandle(unsigned char DATA_AREA *dat);
void HekrModuleControl(unsigned char dat);
void HekrValidDataUpload(unsigned char len);


#endif
