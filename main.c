#include "sys.h"
#include "string.h"
#include "usart.h"
#include "delay.h"
#include "iwdg.h"
#include "timer.h"
#include "led.h"
#include "cJSON.h"
#include "esp8266.h"
#include "onenet.h"
#include "mqttkit.h"
#include "bsp_key.h" 
#include "OLED.h"
#include "adcx.h"
#include "sht30.h"


extern uint8_t key_flag;
float temp,humi;
uint8_t SYSmode = 1,TIMI,Alarm,somk;
uint8_t value_buff[2] = {50,50};





void UsartSendReceive(void);
int main(void)
{
	user_system_init();
	LED_Init();
	Key_GPIO_Config();
	OLED_Init();
	USART1_Config(115200);
	ADCx_Init();
	SHT30_Init();
	key_flag = FLAG_0;
 	while(1)
	{
		
			somk = ADC_To_Percentage(0);
			SHT30_ReadTempHum(&temp, &humi);
		
			switch (key_flag)
        {
        	case FLAG_0:
						OLED_ShowString(0,16,"温:    湿:",OLED_8X16);
						OLED_ShowString(0,32,"霉:    ",OLED_8X16);
						OLED_ShowNum(30,16,temp,2,OLED_8X16);
						OLED_ShowNum(90,16,humi,3,OLED_8X16);
						//OLED_ShowNum(30,32,somk,3,OLED_8X16);
					
					if(somk <= 40){
							OLED_ShowString(30,32,"低",OLED_8X16);
					}else if(somk <= 70){
							OLED_ShowString(30,32,"中",OLED_8X16);
					}else if(somk <= 80){
							OLED_ShowString(30,32,"高",OLED_8X16);
					}
					 
	
						if(KEY3 == 0){
							delay_ms(20);
							if(KEY3 == 0){
								LED = 0;
									JDQ = 0;
								if(SYSmode == 0){
									SYSmode = 1;
								}else{
									SYSmode = 0;
								}
							}
						}
					
					if(SYSmode == 0){//自动模式
							OLED_ShowString(0,48,"    自动",OLED_8X16);
							if(humi >= value_buff[0]){
									LED = 1;
									JDQ = 1;
							}else{
									LED = 0;
									JDQ = 0;
							}
					}else{//手动模式
							OLED_ShowString(0,48,"    手动",OLED_8X16);
						if(KEY1 == 0){
							delay_ms(20);
							if(KEY1 == 0){
								if(LED != 0){
									LED = 0;
								}else{
									LED = 1;
								}
							}
						}
						
						if(KEY2 == 0){
							delay_ms(20);
							if(KEY2 == 0){
								if(JDQ == 0){
									JDQ = 1;
								}else{
									JDQ = 0;
								}
							}
						}
						
						
					}
					
        		break;
        	case FLAG_1:
						OLED_ShowString(0,16,"湿度阈值:",OLED_8X16);
		
						OLED_ShowNum(80,16,value_buff[0],3,OLED_8X16);
						
						if(KEY1 == 0){
							delay_ms(5);
							if(KEY1 == 0){
								value_buff[0] ++;
							}
						}

						if(KEY2 == 0){
							delay_ms(5);
							if(KEY2 == 0){
								value_buff[0] --;
							}
						}
        		break;
        	default:
        		break;
        }

		delay_ms(10);
		OLED_Update();
	}
}


void UsartSendReceive(void)
{
	if(usart_rx.is_packet_ready){
				usart_rx.is_packet_ready = 0;
				char json_str[64];
				memcpy(json_str, usart_rx.buffer, usart_rx.data_length + 1);  
				usart_rx.is_packet_ready = 0;
				//printf("data:%s\n", json_str);
				 cJSON *root = cJSON_Parse(json_str);
				if (root == NULL) {
						const char *error_ptr = cJSON_GetErrorPtr();
						if (error_ptr != NULL) {
							//printf("JSON解析错误: %s\n", error_ptr);
						}
				} else {
					cJSON *LED_switch = cJSON_GetObjectItemCaseSensitive(root, "LED_switch");

					SYSmode = 1;
					if (cJSON_IsNumber(LED_switch)) {
						if(LED_switch->valueint == 0){

						}else{
							
						}
					}
					
						cJSON_Delete(root);
				}
		}
				TIMI ++;
				if(TIMI >= 30){
					TIMI = 0;
						if(Alarm == 0){
							printf("{\"temp\":%.0f,\"humi\":%.0f}",temp,humi);
						}else{
							printf("{\"humi\":1}");
						}
				}
	
}
