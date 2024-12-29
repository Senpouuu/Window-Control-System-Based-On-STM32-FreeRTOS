#include "stm32f10x.h"                  // Device header
float Count = 0;					//计数
uint16_t Round = 8;					//轮子周长 		
float Velocity = 0;					//速度


/*测速中断初始化*/
void SpeedIT_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);		//设置PA6作为中断信号

	/*中断初始化*/
	EXTI_InitTypeDef EXTI_InitStructure;		
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;				//EXIT6中断信号
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//开启中断
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//模式中断
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;	//下降沿触发
	EXTI_Init(&EXTI_InitStructure);
	
	/*配置NVIC优先级*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;		//选择中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//开启
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//配置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//配置响应优先级
	NVIC_Init(&NVIC_InitStructure);
}

void EXTI4_IRQHandler(void)
{
	
	if(EXTI_GetITStatus(EXTI_Line4) == SET)				//判断是否是EXIT14产生的中断
		Count++;
	EXTI_ClearITPendingBit(EXTI_Line4);					//清除中断标志位，防止循环中断
}










