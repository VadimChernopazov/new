/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : my_function.c
  * @brief          : my_function
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include "my_function.h"
#include "math.h"
#include "cmsis_os.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart1;
extern RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN PV */
													
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */
														
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void UART_Printf(const char* fmt, ...) 
{
    char buff[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buff, sizeof(buff), fmt, args);
    HAL_UART_Transmit(&huart3, (uint8_t*)buff, strlen(buff),
                      HAL_MAX_DELAY);
    va_end(args);
}
/********************************************************************************/	
char* make_array_of_coordinate(void)
{
uint8_t serial_1[50];
char*		isym_A=0;
char*		isym_N=0;
char*		isym_E=0;		
float   tmpf_N=0, tmpf_E=0;

 static struct  {
			char N_crd[11];
			char E_crd[11];
			char googl[26];
		}crdnt;
	
do{
HAL_UART_Receive(&huart1, serial_1, 1, HAL_MAX_DELAY);
}
while(strncmp((char*)serial_1,"$", 1));

HAL_UART_Receive(&huart1, serial_1+1, 5, HAL_MAX_DELAY);
if(!(strncmp((char*)serial_1,"$GPRMC",6)))
	{
	HAL_UART_Receive(&huart1, serial_1+6, 50, HAL_MAX_DELAY);
	isym_A=memchr((char*)serial_1,'A',50);
		
		if(!(isym_A==NULL ))
		{
			isym_N=memchr((char*)serial_1,'N',50);
			isym_E=memchr((char*)serial_1,'E',50);
			
			if((isym_A<isym_N)&&(isym_N<isym_E))
				{
				memcpy(crdnt.N_crd, isym_A+2, isym_N-isym_A-3);
				memcpy(crdnt.E_crd, isym_N+2, isym_E-isym_N-2);
						
				isym_N=memchr((char*)crdnt.N_crd,'.',7);
				isym_E=memchr((char*)crdnt.E_crd,'.',7);
							
				tmpf_N=atof(isym_N-2)/60;
				tmpf_E=atof(isym_E-2)/60;
				
				tmpf_N=tmpf_N+atoi(crdnt.N_crd)/100;
				tmpf_E=tmpf_E+atoi(crdnt.E_crd)/100;
					
				sprintf(crdnt.N_crd,"%.3f",tmpf_N);
				sprintf(crdnt.E_crd,"%.3f",tmpf_E);
				//sprintf(crdnt.googl,"%.6f, %.6f",tmpf_N, tmpf_E);//этот формат можно вставл€ть в поисковую строку googl карт. 
				sprintf(crdnt.googl,"%.6f, %.6f,0",tmpf_E, tmpf_N);// Ётот формат дл€ построени€ трека в файлах KML.
				//printf("%.6f, %.6f,0\r\n",tmpf_E, tmpf_N);
				//HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
				//__NOP();
			return (char*)crdnt.googl;
			}
		}	
	}
	//sprintf(crdnt.googl,"%.6f, %.6f,0",56.152306, 57.966560);//заглушка
	//return (char*)crdnt.googl;	
	return false;		
}
/********************************************************************************/	

uint8_t send_message(char* gps_massage)
{	
//char buf[40];	
uint8_t tmp[40];	
char* pointer=0;
	
UART_Printf("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r") ;	
UART_Printf("AT+SAPBR=3,1,\"APN\",\"internet.tele2.ru\"\r") ;	
	//AT+SAPBR=4,1 Ч текущие настройки соединени€
UART_Printf("AT+SAPBR=1,1\r") ;
	
UART_Printf("AT+HTTPINIT\r") ;		
UART_Printf("AT+HTTPPARA=\"CID\",1\r") ;
		
UART_Printf("AT+HTTPPARA=\"URL\",\"freelimelectronic.com/php/sim800l.php?coord=%s&datetime=%s\"\r"\
							,gps_massage, get_RTC_str());
//printf("%s%s\r\n",gps_massage,get_RTC_str());	
HAL_UART_Receive(&huart3, tmp, 40, 2000);
	
UART_Printf("AT+HTTPACTION=0\r") ;//GET = 0, POST =1/
HAL_UART_Receive(&huart3, tmp, 40,  2000);
//printf("%.32s\r\n",tmp);	

//UART_Printf("AT+HTTPREAD\r") ;			
//HAL_UART_Receive(&huart3, &tmp[0], 200, 5000);
UART_Printf("AT+HTTPTERM\r") ;	
UART_Printf("AT+SAPBR=0,1\r") ;

//		vTaskList(buf);
//		printf("%s\r\n",buf);	

pointer=strchr((char*)tmp,'+');	
if(pointer)
	{
	if(strncmp(pointer,"+HTTPACTION: 0,200", 18)==NULL)
		{
		//printf("%.18s\r\n",pointer);
		return true;
		}
	}
//printf("%.32s\r\n",tmp);	
return false;
//__NOP();
}
/**************************************************************************************/	

uint8_t set_RTC(void)
	{
	uint8_t tmp[30];	
	char* pData=0;
	RTC_TimeTypeDef sTime={0,0,0};
	RTC_DateTypeDef sDate={0,0,0};
	
		UART_Printf("AT\r") ;
		UART_Printf("ATE0\r");// отключить эхо
		UART_Printf	("AT+CLTS=1\r");//јктивируем автоматическую синхронизацию времени	
	
		UART_Printf	("AT+CCLK?\r");	
		HAL_UART_Receive(&huart3, tmp, 30, 200);
		pData=strstr((char*)tmp, "+CCLK:");
		printf("strlen pData= %d\r\n",strlen(pData));
		if(strlen(pData)>27)
			{
				pData=pData+8;	

				sDate.Year	=	atoi(pData);
				sDate.Month	=	atoi(pData+3);
				sDate.Date	=	atoi(pData+6);
				
				sTime.Hours	=	atoi(pData+9);
				sTime.Minutes	=	atoi(pData+12);
				sTime.Seconds	=	atoi(pData+15);
				
				HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
				HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);	
			}	
		printf("%.28s\r\n",tmp);			
		if(sDate.Year==4||sDate.Year==0){return true;}
		return false;	
	}
	
	/**************************************************************************************/	
	uint8_t* get_RTC_str(void)
	{
	static char str[19];	
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;	
			
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);	
			
		sprintf(str,"20%.2d.%.2d.%.2d %.2d:%.2d:%.2d"\
		,sDate.Year,  sDate.Month, sDate.Date\
		,sTime.Hours, sTime.Minutes, sTime.Seconds);
	//__NOP();
	//	printf("RTC str=%s\r\n",str);
		
	return (uint8_t*)str;
	}
/**************************************************************************************/		
	void flash_led(uint8_t tmp, uint16_t pause)
	{
		for( ;tmp>0;tmp--)
		{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		HAL_Delay(pause);
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
		HAL_Delay(pause);
		}
	}
	
/**************************************************************************************/	
	void foult_handler(void)
	{
				//char buf[20];
				//char* pointer;
			
				HAL_GPIO_WritePin(RESET_SIM800L_GPIO_Port, RESET_SIM800L_Pin, GPIO_PIN_RESET);
				HAL_Delay(50);	
				HAL_GPIO_WritePin(RESET_SIM800L_GPIO_Port, RESET_SIM800L_Pin, GPIO_PIN_SET);
				HAL_Delay(500);	
		
				UART_Printf("ATE0\r");// отключить эхо
				UART_Printf("AT+CLTS=1\r");//јктивируем автоматическую синхронизацию времени
						
				while(set_RTC());//установить часы	
				printf("Date and time successfully set!\r\n");	
	}

/* USER CODE END 0 */

