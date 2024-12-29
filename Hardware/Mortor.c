#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "OLED.h"
#include "KEY.h"
#include "FreeRTOS.h"
#include "task.h"

extern u8 window;
extern u8 threshold;

void Mortor_Init(void)
{
	PWM_Init();
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_Out_PP ;
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_Initstructure);
}


//100正转停止
void Mortor_ON(void)
{
	threshold = 0;
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
	OLED_ShowString(0,56,"Opening...",8,1);

	//起步
	for (int i = 90; i >= 80; i = i - 5)
	{
		PWM_SetCompare3(i);
		vTaskDelay(pdMS_TO_TICKS(300));
	}
	//加速
	for (int i = 80; i >= 60; i = i - 1)
	{

		PWM_SetCompare3(i);
		vTaskDelay(pdMS_TO_TICKS(160));
	}
	//保持
		vTaskDelay(pdMS_TO_TICKS(1000));
	//减速
	for (int i = 60; i <=80; i=i + 1)
	{

		PWM_SetCompare3(i);
		vTaskDelay(pdMS_TO_TICKS(160));
	}
	//停止
	for (int i = 80 ; i <=100; i=i + 1)
	{

		PWM_SetCompare3(i);
		vTaskDelay(pdMS_TO_TICKS(160));
	}
	window = 1;
	threshold = 1;
	OLED_Clear();
}

//0倒转停止
void Mortor_OFF(void)
{
	threshold = 0;
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	OLED_ShowString(0,56,"Closing...",8,1);
	//起步
	for (int i = 10; i <=20; i=i+1)
	{

		PWM_SetCompare3(i);
		vTaskDelay(pdMS_TO_TICKS(300));
	}
	//加速
	for (int i = 20; i <=40; i=i+1)
	{

		PWM_SetCompare3(i);
		vTaskDelay(pdMS_TO_TICKS(160));
	}
	//保持
		vTaskDelay(pdMS_TO_TICKS(1000));
	//减速
	for (int i = 40; i >=20; i = i - 1)
	{

		PWM_SetCompare3(i);
		vTaskDelay(pdMS_TO_TICKS(160));
	}
	//停止
	for (int i = 20; i>=0; i = i - 1)
	{

		PWM_SetCompare3(i);
		vTaskDelay(pdMS_TO_TICKS(160));
	}
	window = 0;
	threshold = 1;
	OLED_Clear();

}



