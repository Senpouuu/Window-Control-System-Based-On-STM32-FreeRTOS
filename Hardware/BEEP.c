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
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource5);		//设置EXIT1作为中断信号
	
	/*中断初始化*/
	EXTI_InitTypeDef EXTI_InitStructure;		
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;				//EXIT14中断信号
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					//开启中断
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//模式中断
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;	//上升沿触发
	EXTI_Init(&EXTI_InitStructure);
	
	/*配置NVIC优先级*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;	//选择中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//开启
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//配置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0	;		//配置响应优先级
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
	/*如果是低电平，就给他一个相反的信号，高电平同理*/
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5) == 0)
		GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_SET);
	else
		GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_RESET);
}


void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5) == SET)				//判断是否是EXIT14产生的中断
		hc = 1;
	EXTI_ClearITPendingBit(EXTI_Line5);	

}
