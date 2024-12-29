/* User includes. */
#include "stm32f10x.h"                  // Device header
#include "Serial.h"
#include "TimerDelay.h"
#include "LED.h"
#include "OLED.h"
#include "ADC.h"
#include "DHT11.h"
#include "BEEP.h"
#include "MQ-5.h"
#include "Speed.h"
#include "KEY.h"
#include "Mortor.h"

/* Standard includes. */
#include <stdio.h>
#include <math.h>
#include <string.h>


/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"	

/*flags*/
u8 window = 0;
u8 flag = 0;
u8 wifi = 0;
u8 hc = 0;
u8 upload = 0;
u8 serial = 0;

/*RTOS flags*/
static xQueueHandle ADCQueue;
static xSemaphoreHandle UARTMux;
static xSemaphoreHandle UploadSem;
static xTimerHandle SpeedTim;
static xTimerHandle UploadTim;
xTimerHandle OffShakeTim;

/*extern flags*/
extern u16 ADC_vaule[4];
extern float Count;
extern u8 key;


/*Sever*/
const char* ProductID="571107";
const char* DeviceID="1044148397";
const char* Device_AuthID="ESP8266";

/*Threshold values*/
u8 Wind_Threshold = 5;
u16 Sun_Threshold = 8000;
u16 Night_Threshold = 1;
u8 PPM_Threshold = 200;
u16 Rain_Threshold = 800;
u8 threshold = 1;



struct Data
{
	u16 humi;
	u16 temp;
	u16 Wind_Speed;
	u16 PPM;
	u16 Lux;
}WindowData;


static void prvSetupHardware(void)
{
	Serial_Init();
	LED_Init();
	AD_Init();
	OLED_Init();
	DHT11_Init();
	HCSR504_Init();
	SpeedIT_Init();
	KeyIT_Init();
	Mortor_Init();
	OLED_Clear();
}


void TaskCallBack_Upload(TimerHandle_t xTimer)
{
	upload = 1;
}



void TaskCallBack_Speed(TimerHandle_t xTimer)
{
	WindowData.Wind_Speed = ((Count/20)*0.08)/0.2;
	Count = 0;
}


void TaskCallBack_OffShake(TimerHandle_t xTimer)
{
	flag ++;
}

void Task_Mortor_ON(void * param)
{
	Mortor_ON();
	vTaskDelete(NULL);
}

void Task_Mortor_OFF(void * param)
{
	Mortor_OFF();
	vTaskDelete(NULL);
}





void Task_Alarm(void * param)
{
	while(1)
	{
		if(hc == 1)
		{ 	
			BEEP_ON();
			vTaskDelay(pdMS_TO_TICKS(2000));
			xSemaphoreTake(UARTMux, 0);
			ESP8266_Send(0,"BreakTimes",1);	
			xSemaphoreGive(UARTMux);
			hc = 0;
			BEEP_OFF();
		}
		vTaskDelay(50);
	}	
}

void Task_ConnectWIFI(void * param)
{

	while(1)
	{
		while(wifi == 0)
		{
			if (strstr((const char*)Serial_RxPacket,"WIFI GOT IP"))				//等待wifi连接
			{
				wifi = 1;
				break;
			}
			vTaskDelay(20);
		}
		xSemaphoreTake(UARTMux, portMAX_DELAY);
		printf("AT+IOTCFG=%s,%s,%s\r\n",DeviceID,ProductID,Device_AuthID);							//配置地址
		vTaskDelay(pdMS_TO_TICKS(2000));


		printf("AT+IOTSUB=Window_Switch\r\n");													//配置地址
		xSemaphoreGive(UARTMux);
		vTaskDelay(pdMS_TO_TICKS(2000));
		
		LED6_ON();
		xSemaphoreGive(UploadSem);
		vSemaphoreDelete(UploadSem);

		memset(Serial_RxPacket,0,500);
		vTaskDelete(NULL);
		
	}


}


void Task_Treshold(void * param)
{
	while(1)
	{

		//PPM
		if(threshold == 1 && window == 0)			//条件开启，且窗户关闭
			if(WindowData.PPM > PPM_Threshold)
			{
				Mortor_ON();
				vTaskDelay(pdMS_TO_TICKS(40));
				xSemaphoreTake(UARTMux, 0);
				ESP8266_Send(0,"LogTag",2);	
				xSemaphoreGive(UARTMux);
			}


		//Lux
		if(threshold == 1 && window == 0)			//条件开启，且窗户关闭
			if(WindowData.Lux > Sun_Threshold)
			{
				Mortor_ON();
				vTaskDelay(pdMS_TO_TICKS(40));
				xSemaphoreTake(UARTMux, 0);
				ESP8266_Send(0,"LogTag",4);	
				xSemaphoreGive(UARTMux);
			}
		//开->关
		if(threshold == 1 && window == 1)			//条件开启，且窗户关闭
			if(WindowData.Lux < Night_Threshold)
			{
				Mortor_OFF();
				vTaskDelay(pdMS_TO_TICKS(40));
				xSemaphoreTake(UARTMux, 0);
				ESP8266_Send(0,"LogTag",5);	
				xSemaphoreGive(UARTMux);

			}

		//Wind
		if(threshold == 1 && window == 1)			//条件开启，且窗户关闭
			if(WindowData.Wind_Speed > Wind_Threshold)
			{
				Mortor_OFF();
				vTaskDelay(pdMS_TO_TICKS(40));
				xSemaphoreTake(UARTMux, 0);
				ESP8266_Send(0,"LogTag",3);	
				xSemaphoreGive(UARTMux);
			}

		//Rain
		if(threshold == 1 && window == 1)			//条件开启，且窗户关闭
			if(ADC_vaule[3] < Rain_Threshold)
			{
				Mortor_OFF();
				vTaskDelay(pdMS_TO_TICKS(40));
				xSemaphoreTake(UARTMux, 0);
				ESP8266_Send(0,"LogTag",1);	
				xSemaphoreGive(UARTMux);

			}

		vTaskDelay(10);
	}

}

void Task_Monitor(void * param)
{
	while(1)
	{
		//upload
		if(upload == 1)
		{
			xSemaphoreTake(UploadSem,0);
			xSemaphoreTake(UARTMux, portMAX_DELAY);
			ESP8266_Send(0,"Temp",WindowData.temp);
			vTaskDelay(pdMS_TO_TICKS(80));
			
			ESP8266_Send(0,"Humi",WindowData.humi);
			vTaskDelay(pdMS_TO_TICKS(80));
			
			ESP8266_Send(0,"WindSpeed",WindowData.Wind_Speed);
			vTaskDelay(pdMS_TO_TICKS(80));
			
			ESP8266_Send(0,"PPM",WindowData.PPM);				
			vTaskDelay(pdMS_TO_TICKS(80));
			
			ESP8266_Send(0,"Lux",WindowData.Lux);	
			vTaskDelay(pdMS_TO_TICKS(80));
			
			ESP8266_Send(0,"WindowSwitch",window);	
			vTaskDelay(pdMS_TO_TICKS(80));
			xSemaphoreGive(UARTMux);
			upload = 0;
			//printf("%s\r\n",Serial_RxPacket);
		}


		//KEY
		if(key == 1)
		{
			if(window == 0)
			{
				xTaskCreate(Task_Mortor_ON, "MortorON", 100, NULL, 1, NULL);
				xSemaphoreTake(UARTMux, 0);
				ESP8266_Send(0,"LogTag",7);	
				xSemaphoreGive(UARTMux);
			}
				
			else
			{
				xTaskCreate(Task_Mortor_OFF, "MortorOFF", 100, NULL, 1, NULL);
				xSemaphoreTake(UARTMux, 0);
				ESP8266_Send(0,"LogTag",6);	
				xSemaphoreGive(UARTMux);
			}
			key = 0;
		}
		
		
		//Serial
		if(serial == 1)
		{
			xTaskCreate(Task_Mortor_ON, "MortorON", 100, NULL, 1, NULL);
			serial = 0;
			xSemaphoreTake(UARTMux, 0);
			ESP8266_Send(0,"LogTag",7);	
			xSemaphoreGive(UARTMux);

		}
		else if(serial == 2)
		{
			xTaskCreate(Task_Mortor_OFF, "MortorOFF", 100, NULL, 1, NULL);
			serial = 0;
			xSemaphoreTake(UARTMux, 0);
			ESP8266_Send(0,"LogTag",6);	
			xSemaphoreGive(UARTMux);
		}

		


		vTaskDelay(20);
	}

}





void Task_DHT11_GetData(void * param)
{
	DHT11_Read_data(&humi,&temp,&dechumi,&dectemp);
	vTaskDelete(NULL);

}



void Task_Delay(void * param)
{
	while(1)
	{
		xTaskCreate(Task_DHT11_GetData, "DHT",100, NULL, 3, NULL);
		vTaskDelay(50);
	}
	
}

void Task_GetData(void * param)
{
	while (1)
	{

		WindowData.humi = humi;
		xQueueSend(ADCQueue, &WindowData.humi, portMAX_DELAY);

		WindowData.temp = temp;
		xQueueSend(ADCQueue, &WindowData.temp, portMAX_DELAY);
	
		xQueueSend(ADCQueue, &WindowData.Wind_Speed, portMAX_DELAY);

		WindowData.PPM = (int)trunc(MQ5_CalPPM(80.897, -2.431));
		xQueueSend(ADCQueue, &WindowData.PPM, portMAX_DELAY);

		WindowData.Lux = 39810*pow(2.71828,-0.01534*ADC_vaule[2])+3467*pow(2.71828,-0.002379*ADC_vaule[2]);
		xQueueSend(ADCQueue, &WindowData.Lux, portMAX_DELAY);

		
		vTaskDelay(10);
	}
}


void Task_ShowData(void * param)
{
	u16 humi;
	u16 temp;
	u16 Wind_Speed;
	u16 PPM;
	u16 Lux;
	while(1)
	{
		xQueueReceive(ADCQueue,&humi,portMAX_DELAY);
		OLED_ShowNum(0,0,humi,4,8,1);
		
		xQueueReceive(ADCQueue,&temp,portMAX_DELAY);
		OLED_ShowNum(0,12,temp,4,8,1);
		
		xQueueReceive(ADCQueue,&Wind_Speed,portMAX_DELAY);
		OLED_ShowNum(0,24,Wind_Speed,4,8,1);
		
		xQueueReceive(ADCQueue,&PPM,portMAX_DELAY);
		OLED_ShowNum(0,36,PPM,4,8,1);

		
		xQueueReceive(ADCQueue,&Lux,portMAX_DELAY);
		OLED_ShowNum(0,48,Lux,4,8,1);


		OLED_Refresh();

		vTaskDelay(100);

	}
	

}



int main(void)
{
	prvSetupHardware();

	ADCQueue = xQueueCreate(5,sizeof(u16));
	UARTMux = xSemaphoreCreateMutex();
	UploadSem = xSemaphoreCreateBinary();
	SpeedTim = xTimerCreate("SpeedTimer",pdMS_TO_TICKS(200),pdTRUE,NULL,TaskCallBack_Speed);
	OffShakeTim = xTimerCreate("OffShakeTimer",pdMS_TO_TICKS(10),pdFALSE,NULL,TaskCallBack_OffShake);
	UploadTim = xTimerCreate("UploadTimer",pdMS_TO_TICKS(2000),pdTRUE,NULL,TaskCallBack_Upload);
	
	xTimerStart(SpeedTim,portMAX_DELAY);
	xTimerStart(OffShakeTim,portMAX_DELAY);
	xTimerStart(UploadTim,portMAX_DELAY);

	
	xTaskCreate(Task_GetData, "DataTask", 100, NULL, 1, NULL);

	xTaskCreate(Task_ShowData, "ShowTask", 200, NULL, 1, NULL);
	
	xTaskCreate(Task_Delay, "DelayTask", 100, NULL, 2, NULL);

	xTaskCreate(Task_Alarm, "AlarmTask", 100, NULL, 1, NULL);

	xTaskCreate(Task_Monitor, "MonitorTask", 100, NULL, 1, NULL);

	xTaskCreate(Task_ConnectWIFI, "CWIFITask", 100, NULL, 1, NULL);
	
	xTaskCreate(Task_Treshold, "ThresholdTask", 100, NULL, 1, NULL);

	vTaskStartScheduler();

	return 0;
	

}

