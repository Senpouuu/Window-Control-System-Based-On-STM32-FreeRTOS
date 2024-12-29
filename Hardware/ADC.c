#include "stm32f10x.h"                  // Device header
u16 ADC_vaule[4];

void DriectMemoryAccess_Init(void)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	DMA_InitTypeDef DMA_InitStructure;
	/*外设*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;				//数据1基地址 ADC1寄存器地址
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//16位
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//自增关闭
	/*存储器*/
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_vaule;					//数据2基地址	
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//字节方式传输
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						//自增开启
	DMA_InitStructure.DMA_BufferSize = 4;										//传输次数
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;							//传输方向（存储器到存储器）
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								//硬件触发or软件触发 (ADC1触发)
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;								//循环模式（存储器到存储器不能使用自动重装）
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;							//优先级高
	DMA_Init(DMA1_Channel1,&DMA_InitStructure);									//ADC1 连 DMA1
	
	ADC_DMACmd(ADC1,ENABLE);													//ADC1 DMA触发源开启
	DMA_Cmd(DMA1_Channel1,ENABLE);

}


void DMA_Reset(void)
{
	DMA_Cmd(DMA1_Channel1,DISABLE);												//关闭
	DMA_SetCurrDataCounter(DMA1_Channel1,4);									//重置计数器
	DMA_Cmd(DMA1_Channel1,ENABLE);	
}



void AD_Init(void)
{	
	/*RCC开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); 		//72/6 = 12Mhz
	
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	//ADC模拟输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*ADC配置*/
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;							//设置为ADC独立模式							
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;						//数据右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;			//不使用外部触发模式
	/*转换模式配置（单次转换非扫描）*/
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;							//单次转换 	
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;								//非扫描模式
	ADC_InitStructure.ADC_NbrOfChannel = 4;										//4个通道	
		
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_55Cycles5);	//设置规则组通道菜单
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_55Cycles5);	//设置规则组通道菜单
	ADC_RegularChannelConfig(ADC1,ADC_Channel_2,3,ADC_SampleTime_55Cycles5);	//设置规则组通道菜单
	ADC_RegularChannelConfig(ADC1,ADC_Channel_3,4,ADC_SampleTime_55Cycles5);	//设置规则组通道菜单
	ADC_Init(ADC1,&ADC_InitStructure);

	ADC_Cmd(ADC1,ENABLE);														//上电

	/*ADC校准*/
	ADC_ResetCalibration(ADC1);													//开始复位校准
	while(ADC_GetResetCalibrationStatus(ADC1) == SET);							//等待复位校准完成
	ADC_StartCalibration(ADC1);													//开始校准
	while(ADC_GetCalibrationStatus(ADC1) == SET);								//等待校准完成

	ADC_SoftwareStartConvCmd(ADC1,ENABLE);										//开启软件转换
	DriectMemoryAccess_Init();
}







