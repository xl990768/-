/**
 ****************************************************************************************************
 * @author      创想电子
 ****************************************************************************************************
 * @attention
 * 主控系统:STM32F103C8T6最小系统板
 * 联系方式(VX):liuhahahahaha-
 ****************************************************************************************************
 */
#include "adcx.h"
#include "delay.h"
		   		 
																   
void  ADCx_Init(void)
{ 	
	ADC_InitTypeDef   ADC_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(PulseSensor_GPIO_CLK|ADC_CLK,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	//PA0
	GPIO_InitStructure.GPIO_Pin=PulseSensor_GPIO_PIN | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AIN;
	GPIO_Init(PulseSensor_GPIO_PORT,& GPIO_InitStructure);
	
	ADC_DeInit(ADCx);
	
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADCx和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	      //模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	  //转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign= ADC_DataAlign_Right;                	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	                              //顺序进行规则转换的ADC通道的数目
	ADC_Init(ADCx, &ADC_InitStructure);	
	
//	ADC_ExternalTrigConvCmd(ADCx,ENABLE);//采用外部触发
	ADC_RegularChannelConfig(ADCx, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5);//adc转换时间21us
	ADC_Cmd(ADCx, ENABLE);
	
	ADC_ResetCalibration(ADCx);									//复位校准
	while(ADC_GetResetCalibrationStatus(ADCx));	//等待校准结束，校准结束状态为RESET
	ADC_StartCalibration(ADCx);									//AD校准
	while(ADC_GetCalibrationStatus(ADCx));			//等待校准结束	

}				  
//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADCx, ch, 1, ADC_SampleTime_239Cycles5 );	//ADCx,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADCx, ENABLE);		//使能指定的ADCx的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADCx);	//返回最近一次ADCx规则组的转换结果
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 	 



float PH_GetData(void)
{
	float  tempData = 0;
	float PH_DAT;
 
	for (uint8_t i = 0; i < 10; i++)
	{
		tempData += Get_Adc(1);
		delay_ms(5);
	}
	tempData /= 10;
	
	PH_DAT = (tempData/4096.0)*3.3;
	PH_DAT = -5.7541*PH_DAT+16.654;
	
	if(PH_DAT>14.0)PH_DAT=14.0;
	if(PH_DAT<0)PH_DAT=0.0;
	
	return PH_DAT;
}



float WATER_GetData(void){	
	uint32_t tempData = 0;
	for (uint8_t i = 0; i < 10; i++)	
	{
	tempData += Get_Adc(4);		
	delay_ms(5);	
	}
	tempData /= 10;	
	
	return (float)tempData * 6.0f / 4095.0f;	
}


/**
  * @brief  将ADC原始值转换为百分比 (0-100%)
  * @param  adc_value: ADC原始值 (0-4096)
  * @retval 百分比值 (0.0 - 100.0)
  */
uint8_t ADC_To_Percentage(u8 ch)
{
    uint8_t percentage;
    u16 adc_value = Get_Adc_Average(ch,5);
    // 确保输入值在有效范围内
    if(adc_value > 4096) {
        adc_value = 4096;
    }
    
    // 转换为百分比
    percentage = ((float)adc_value / 4096.0f) * 100.0f;
    
    return percentage;
}


/****
*******DHT11初始化函数
*******返回1:初始化失败
*******返回0:初始化成功
*****/
void PM25_Init(void)
{	 
	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(PM25_LED_GPIO_CLK_ENABLE, ENABLE);	//使能端口时钟
	GPIO_InitStructure.GPIO_Pin = PM25_LED_PIN;				 				//引脚配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 			//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 			//IO口速度为50MHz
	GPIO_Init(PM25_LED_PORT, &GPIO_InitStructure);					 	//根据设定参数初始化
	GPIO_SetBits(PM25_LED_PORT,PM25_LED_PIN);						 			//IO口输出高
}

/****
*******获取一次PM2.5值函数
*******返回值：PM2.5值
*******注意：本函数需要调用adc获取函数
*****/
float GetGP2Y(void)
{
	uint32_t AD_PM;  
	double pm; 
	PM25_LED = 0;
	delay_us(280);
	AD_PM = Get_Adc(ADC_Channel_0);
	delay_us(40);
	PM25_LED = 1;
	delay_us(9680);
	if(AD_PM > 30)
		pm = ((AD_PM/4096.0)*3.3*0.17-0.1)*1000;		//计算PM2.5值
	else
		pm = 0;
	return pm;
} 

/****
*******获取平均times次PM2.5值函数
*******传入值：取多少次
*******返回值：平均的PM2.5值
*******注意：本函数需要调用adc获取函数
*****/
uint16_t Pm25_Get_Value(uint8_t times)
{
	uint32_t pm25_val=0;
	uint8_t t;
	for(t=0; t<times; t++)
	{
		pm25_val+=GetGP2Y();
		delay_ms(5);
	}
	return pm25_val/times;
} 

