#include <reg52.h>        
#include "hekr_protocol.h"

#define USER_REV_BUFF_MAX_LEN		20	// ����ʵ�������������buffer��С

void InitUART(void);
void SendByte(unsigned char dat);
unsigned char xdata Recv_Buffer[USER_REV_BUFF_MAX_LEN];  //�洢����
unsigned char Recv_STA = 0;
/*------------------------------------------------
                    �������
------------------------------------------------*/
void main (void)
{
	unsigned char temp = 0,UserValidLen = 9;
	InitUART();//��ʼ������
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
				//���յ����ݱ����� valid_data ������
				//User Code
				SendByte(valid_data[0]);
			}
			if(HekrModuleStateUpdate == temp)
			{
				//���յ����ݱ����� ModuleStatus ָ����
				//User Code.
				SendByte(ModuleStatus->CMD);
			}
			Recv_STA = 0;		
		}
	}
}

void InitUART(void)
{
	SCON  = 0x50;		  //������9600       
  TMOD |= 0x20;              
  TH1   = 0XF3;             
	TL1   = 0XF3;
  TR1   = 1;                                       
  EA    = 1;                  
  ES    = 1;                
}         

void SendByte(unsigned char dat)//�򴮿ڷ�������
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
			if (HEKR_FRAME_HEADER == Temp)//���������0x48 �޷�������  ����֡ͷ�Ϳ�ʼ����������
			{
				count = 0;
				flag = 1;  //��־λ��1
				Recv_Buffer[count++] = Temp;
			}
		}
		else//����ͷ����֮��Ϳ�ʼ�洢��������ݶ���
		{
			if (USER_REV_BUFF_MAX_LEN > count)//���ݸ���û�г��������С
			{
				Recv_Buffer[count++] = Temp;//�洢����
				if((count > 4) && (count >= Recv_Buffer[1]))
				//������ݸ�������4 �����ݸ������ڵ���֡��0x0e�͸�λ�����¶�������
				{
					Recv_STA = 1;
					flag = 0;
					count = 0;
				}
			}
			else//�洢�����ݸ�����������涨�ʹ��¶�������
			{
				flag = 0;
				count = 0;
			}
		}
	}
} 
