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

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource13);		//设置EXIT14作为中断信号

	/*中断初始化*/
	EXTI_InitTypeDef EXTI_InitStructure;		
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;				//EXIT14中断信号
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//开启中断
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//模式中断
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;	//下降沿触发
	EXTI_Init(&EXTI_InitStructure);
	
	/*配置NVIC优先级*/
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;	//选择中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//开启
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//配置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//配置响应优先级
	NVIC_Init(&NVIC_InitStructure);

}


void KeyIT_SwiInit(FunctionalState State)
{
	EXTI_InitTypeDef EXTI_InitStructure;	
	EXTI_InitStructure.EXTI_LineCmd = State;				//开启中断
	EXTI_Init(&EXTI_InitStructure);
}


/*中断服务函数，这里的中断服务函数必须与启动文件中的名称相同*/
void EXTI15_10_IRQHandler(void)
{

	if(EXTI_GetITStatus(EXTI_Line13) == SET)				//判断是否是EXIT14产生的中断
	{
		xTimerResetFromISR(OffShakeTim, 0);
		if(flag >= 2)
		{
			key = 1;
			flag = 0;
		}

	}
	EXTI_ClearITPendingBit(EXTI_Line13);				//清除中断标志位，防止循环中断

}
