#include "stm32f10x.h"                  // Device header
#include "LED.h"
#include "FreeRTOS.h"
#include "timers.h"	


extern u8 flag;
extern xTimerHandle OffShakeTim;
u8 key = 0;


void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}



void KeyIT_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource13);		//����EXIT14��Ϊ�ж��ź�

	/*�жϳ�ʼ��*/
	EXTI_InitTypeDef EXTI_InitStructure;		
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;				//EXIT14�ж��ź�
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//�����ж�
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//ģʽ�ж�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;	//�½��ش���
	EXTI_Init(&EXTI_InitStructure);
	
	/*����NVIC���ȼ�*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;	//ѡ���ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//����
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//������ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//������Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStructure);

}


void KeyIT_SwiInit(FunctionalState State)
{
	EXTI_InitTypeDef EXTI_InitStructure;	
	EXTI_InitStructure.EXTI_LineCmd = State;				//�����ж�
	EXTI_Init(&EXTI_InitStructure);
}


/*�жϷ�������������жϷ����������������ļ��е�������ͬ*/
void EXTI15_10_IRQHandler(void)
{

	if(EXTI_GetITStatus(EXTI_Line13) == SET)				//�ж��Ƿ���EXIT14�������ж�
	{
		xTimerResetFromISR(OffShakeTim, 0);
		if(flag >= 2)
		{
			key = 1;
			flag = 0;
		}

	}
	EXTI_ClearITPendingBit(EXTI_Line13);				//����жϱ�־λ����ֹѭ���ж�

}
