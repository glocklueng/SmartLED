#include <reg52.h>        
#include "hekr_protocol.h"

#define USER_REV_BUFF_MAX_LEN		20	// ¸ù¾İÊµ¼ÊÇé¿öµ÷Õû½ÓÊÕbuffer´óĞ¡

void InitUART(void);
void SendByte(unsigned char dat);
unsigned char xdata Recv_Buffer[USER_REV_BUFF_MAX_LEN];  //´æ´¢Êı¾İ
unsigned char Recv_STA = 0;
/*------------------------------------------------
                    Ö÷º¯Êı°
------------------------------------------------*/
void main (void)
{
	unsigned char temp = 0,UserValidLen = 9;
	InitUART();//³õÊ¼»¯´®¿Ú
	HekrInit(SendByte);
	HekrValidDataUpload(UserValidLen);
  	HekrModuleControl(HekrConfig);
	while(1)
	{
		if(Recv_STA)
		{
			temp = HekrRecvDataHandle(Recv_Buffer);
			if(ValidDataUpdate == temp)
			{
				//½ÓÊÕµÄÊı¾İ±£´æÔÚ valid_data Êı×éÀï
				//User Code
				SendByte(valid_data[0]);
			}
			if(HekrModuleStateUpdate == temp)
			{
				//½ÓÊÕµÄÊı¾İ±£´æÔÚ ModuleStatus Ö¸ÕëÀïŒ
				//User Code.
				SendByte(ModuleStatus->CMD);
			}
			Recv_STA = 0;		
		}
	}
}

void InitUART(void)
{
	SCON  = 0x50;		  //²¨ÌØÂÊ9600       
  TMOD |= 0x20;              
  TH1   = 0XF3;             
	TL1   = 0XF3;
  TR1   = 1;                                       
  EA    = 1;                  
  ES    = 1;                
}         

void SendByte(unsigned char dat)//Ïò´®¿Ú·¢ËÍÊı¾İ
{
	SBUF = dat;
	while(!TI);
		TI = 0;
}

void UART_SER (void) interrupt 4 
{
	unsigned char Temp;        
	static unsigned char count,flag = 0;
	
	if (RI)                      
	{
  		RI = 0;                      
		Temp = SBUF;

		if (0 == flag)
		{
			if (HEKR_FRAME_HEADER == Temp)//Èç¹ûÊı¾İÊÇ0x48 ÎŞ·ûºÅÀàĞÍ  Êı¾İÖ¡Í·¾Í¿ªÊ¼´æºóÃæµÄÊı¾İ
			{
				count = 0;
				flag = 1;  //±êÖ¾Î»ÖÃ1
				Recv_Buffer[count++] = Temp;
			}
		}
		else//Êı¾İÍ·¶ÔÁËÖ®ºó¾Í¿ªÊ¼´æ´¢ºóÃæµÄÊı¾İ¶ÔÁË
		{
			if (USER_REV_BUFF_MAX_LEN > count)//Êı¾İ¸öÊıÃ»ÓĞ³¬¹ıÊı×é´óĞ¡
			{
				Recv_Buffer[count++] = Temp;//´æ´¢Êı¾İ
				if((count > 4) && (count >= Recv_Buffer[1]))
				//Èç¹ûÊı¾İ¸öÊı´óÓÚ4 ºÍÊı¾İ¸öÊı´óÓÚµÈÓÚÖ¡³¤0x0e¾Í¸´Î»£¬´ÓĞÂ¶ÁÈëÊı¾İ
				{
					Recv_STA = 1;
					flag = 0;
					count = 0;
				}
			}
			else//´æ´¢µÄÊı¾İ¸öÊı´óÓÚÊı×é¹æ¶¨¾Í´ÓĞÂ¶ÁÈëÊı¾İ
			{
				flag = 0;
				count = 0;
			}
		}
	}
} 
