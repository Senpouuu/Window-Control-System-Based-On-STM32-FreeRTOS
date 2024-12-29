#include "stm32f10x.h"                  // Device header
extern u8 hc;

void Beep_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
	
}

void HCSR504_Init(void)
{
	Beep_Init();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource5);		//����EXIT1��Ϊ�ж��ź�
	
	/*�жϳ�ʼ��*/
	EXTI_InitTypeDef EXTI_InitStructure;		
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;				//EXIT14�ж��ź�
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					//�����ж�
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//ģʽ�ж�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	//�����ش���
	EXTI_Init(&EXTI_InitStructure);
	
	/*����NVIC���ȼ�*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;	//ѡ���ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//����
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//������ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0	;		//������Ӧ���ȼ�
	NVIC_Init(&NVIC_InitStructure);
}


void BEEP_ON(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);
}


void BEEP_OFF(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_5);
}

 void BEEP_Turn(void)
{
	/*����ǵ͵�ƽ���͸���һ���෴���źţ��ߵ�ƽͬ��*/
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) == 0)
		GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_SET);
	else
		GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_RESET);
}


void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5) == SET)				//�ж��Ƿ���EXIT14�������ж�
		hc = 1;
	EXTI_ClearITPendingBit(EXTI_Line5);	

}
