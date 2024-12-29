#include "stm32f10x.h"                  // Device header
float Count = 0;					//����
uint16_t Round = 8;					//�����ܳ� 		
float Velocity = 0;					//�ٶ�


/*�����жϳ�ʼ��*/
void SpeedIT_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);		//����PA6��Ϊ�ж��ź�

	/*�жϳ�ʼ��*/
	EXTI_InitTypeDef EXTI_InitStructure;		
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;				//EXIT6�ж��ź�
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//�����ж�
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//ģʽ�ж�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	//�½��ش���
	EXTI_Init(&EXTI_InitStructure);
	
	/*����NVIC���ȼ�*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;		//ѡ���ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//����
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//������ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//������Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStructure);
}

void EXTI4_IRQHandler(void)
{
	
	if(EXTI_GetITStatus(EXTI_Line4) == SET)				//�ж��Ƿ���EXIT14�������ж�
		Count++;
	EXTI_ClearITPendingBit(EXTI_Line4);					//����жϱ�־λ����ֹѭ���ж�
}










