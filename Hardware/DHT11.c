#include "stm32f10x.h"                  // Device heade
#include "DHT11.h"
#include "OLED.h"
#include "TimerDelay.h"
u8 count;
u8 humi,temp,dechumi,dectemp;


/*GPIO开启时钟*/
void DHT11_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
}



/*配置dht11(GPIO)模式，1输出 0输入*/
void DHT_Mode(u8 mode)
{
	GPIO_InitTypeDef GPIO_InitStrucuture;
	if(mode)
	{
		GPIO_InitStrucuture.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStrucuture.GPIO_Pin = DHT11;
		GPIO_InitStrucuture.GPIO_Speed = GPIO_Speed_50MHz;
	}
	else
	{
		GPIO_InitStrucuture.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_InitStrucuture.GPIO_Pin = DHT11;
		GPIO_InitStrucuture.GPIO_Speed = GPIO_Speed_50MHz;
	}
	GPIO_Init(DHT11_Port,&GPIO_InitStrucuture);
}



/*DHT11开始信号*/
void DHT_Sends_Start(void)
{
	DHT_Mode(1);												//GPIO输出
	GPIO_ResetBits(DHT11_Port,DHT11);							//拉低
	Delay_ms(18);												//等待18ms
	GPIO_SetBits(DHT11_Port,DHT11);								//拉高
	Delay_us(40);												//等待40us
}



/*等待DHT11响应完成*/
void DHT_Sends_Response(void)
{
	DHT_Mode(0);												//GPIO输入

	/*等待低电平结束*/
	while((GPIO_ReadInputDataBit(DHT11_Port,DHT11) == 0 ) && (count<100))
	{
		count++;
		Delay_us(1);										
	}
	count=0;

	/*等待高电平结束*/
	while((GPIO_ReadInputDataBit(DHT11_Port,DHT11) == 1 ) && (count<100))
	{
		count++;
		Delay_us(1);										
	}
	count=0;
}




/*DHT11读数据位*/
u8 DHT11_Read_Byte(void)
{
	u8 data = 0;
	for(u8 i=0;i<8;i++)
	{
		DHT_Mode(0);											//GPIO输入

		/*等待低电平结束*/
		while((GPIO_ReadInputDataBit(DHT11_Port,DHT11) == 0 ) && (count<100))
		{
			count++;
			Delay_us(1);											
		}
		count=0;
			
		data<<=1;													//data移位初始化
		
		/*写数据1*/
		Delay_us(40);												//高电平40us		
		if(GPIO_ReadInputDataBit(DHT11_Port,DHT11) == 1)
		{
			data |= 0x01;											//低位移高位
			/*等待高电平结束*/
			while((GPIO_ReadInputDataBit(DHT11_Port,DHT11) == 1 ) && (count<100))
			{
				count++;
				Delay_us(1);											
			}
			count=0;
		}
	}
	return data;
}




/*DHT11数据位转换*/
void DHT11_Read_data(u8 *humi,u8 *temp,u8 *dechumi,u8 *dectemp)
{
	u8 DATA[5]={0,0,0,0,0};											//初始化

	DHT_Sends_Start();
	DHT_Sends_Response();

	for(u8 i=0;i<5;i++)
	{
		DATA[i] = DHT11_Read_Byte();
	}
	Delay_us(200);													//等待50us

	/*湿度+温度校验*/
	if((DATA[0]+DATA[1]+DATA[2]+DATA[3]) == DATA[4])
	{
		*humi = DATA[0];
		*dechumi = DATA[1];
		*temp = DATA[2];
		*dectemp = DATA[3];
	}
	else
	{
		for(u8 i=0;i<5;i++)
		DATA[i] = 0;
	}
}





