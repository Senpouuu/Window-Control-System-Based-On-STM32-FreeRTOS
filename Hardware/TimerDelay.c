#include "stm32f10x.h"                  // Device header


void Delay_us(int us)
{
	/*配置TIM参数*/

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);			//开启TIM2时钟信号
	TIM_InternalClockConfig(TIM2);								//选择内部TIM2时钟
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;			//选择分频
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//选择向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period = us - 1;						//1ms
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1;					
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;				//关闭重复计数器
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	TIM_Cmd(TIM2,ENABLE);
	
	TIM_SetCounter(TIM2,0);
	while(TIM_GetCounter(TIM2) != (us - 1));

	TIM_Cmd(TIM2,DISABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,DISABLE);			//开启TIM2时钟信
			
}

void Delay_ms(int ms)
{
	while(ms--)
	{
		Delay_us(1000);
	}
}
 
void Delay_s(int s)
{
	while(s--)
	{
		Delay_ms(1000);
	}
} 
