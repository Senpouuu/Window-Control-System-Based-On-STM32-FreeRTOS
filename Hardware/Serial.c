#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include "string.h"
#include <stdarg.h>

char Serial_RxPacket[500];
int pRxPacket;
extern u8 serial;

void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_AF_PP ;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure); 

	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IPU ;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure); 
		
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 115200;											//波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;						//串口模式（发送）+ （接收）
	USART_InitStructure.USART_Parity = USART_Parity_No;									//校验模式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;								//停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;							//字长
	USART_Init(USART1,&USART_InitStructure);

	/*串口中断配置*/
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);										//开启RXNE标志位中断
	/*配置NVIC中断优先级*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);			//二级组优先
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		//选择USART1中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//开启
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;//配置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//配置响应优先级
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1,ENABLE);
}


void Serial_DeInit(void)
{
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);										//开启RXNE标志位中断
}



void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1,Byte);														//发送数据
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);							//等待数据进入寄存器
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}


void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}


/*printf重定向*/
int fputc(int ch,FILE *f )   
{
	USART_SendData(USART1,(uint8_t)ch);
	while(USART_GetFlagStatus (USART1,USART_FLAG_TC) == RESET);
	return ch;
}


/*ESP发送数据
指令AT+IOTSEND=a,b,c,(d)
a：0：数据是数值类型
1：数据是字符串类型
2：数据是gps
b：数据流名
c：数据值
d：数据值 (只有上传GPS时才会用到)*/
void ESP8266_Send(uint8_t a,char* b,float c)
{
	printf("AT+IOTSEND=%d,%s,%.1f\r\n",a,b,c);												
}



/*不带指令判断的中断函数*/
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
	//Serial_RxFlag = 1;
	//memset(Serial_RxPacket,0,500);														//接收区清零`
		Serial_RxPacket[pRxPacket ++]=USART_ReceiveData(USART1);
		if((Serial_RxPacket[pRxPacket - 2] == '\r')|(Serial_RxPacket[pRxPacket - 1] == '\n'))  
		{
			Serial_RxPacket[pRxPacket - 2] = '\0';
			pRxPacket = 0;
		}
	
		if (strstr((const char*)Serial_RxPacket,"LED_ON"))			//点灯指令
		{
			serial = 1;
			memset(Serial_RxPacket,0,500);
		}
		if (strstr((const char*)Serial_RxPacket,"LED_OFF"))		//灭灯指令
		{
			serial = 2;
			memset(Serial_RxPacket,0,500);
		}
	}
}


