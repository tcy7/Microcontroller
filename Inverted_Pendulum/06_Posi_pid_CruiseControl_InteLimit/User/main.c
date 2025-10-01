#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Timer.h"
#include "Key.h"
#include "RP.h"
#include "Motor.h"
#include "Serial.h"
#include "Encoder.h"
#include <math.h>

#define EPSILON 0.0001

uint8_t KeyNum;
uint8_t MotorFlag=1;

float Actual,Target,Out;
float Kp,Ki,Kd;
float Error0,Error1,ErrorIntOut;



int main(void)
{
	OLED_Init();
	Key_Init();
	Motor_Init();	
	Encoder_Init();
	Timer_Init();
	Serial_Init();
	
	OLED_Printf(0, 0, OLED_8X16, "Speed Control");
	OLED_Update();
	
	while (1)
	{
		KeyNum = Key_GetNum();	
		
		if (KeyNum == 1)			
		{
			MotorFlag=!MotorFlag;		
		}
		
		if(MotorFlag){
			LED_ON();
		}else{
			LED_OFF();
		}
		
		Kp=RP_GetValue(1)/4095*2;
		Ki=RP_GetValue(2)/4095*2;
		Kd=RP_GetValue(3)/4095*2;
		Target=RP_GetValue(4)/ 4095.0 * 300 - 150;
		
		OLED_Printf(0, 16, OLED_8X16, "Kp:%4.2f", Kp);		
		OLED_Printf(0, 32, OLED_8X16, "Ki:%4.2f", Ki);		
		OLED_Printf(0, 48, OLED_8X16, "Kd:%4.2f", Kd);		
		OLED_Printf(64, 16, OLED_8X16, "Tar:%+04.0f", Target);	
		OLED_Printf(64, 32, OLED_8X16, "Act:%+04.0f", Actual);	
		OLED_Printf(64, 48, OLED_8X16, "Out:%+04.0f", Out);		
		
		OLED_Update();
	
		Serial_Printf("%f,%f,%f\r\n",Target,Actual,Out);
	}
}

void TIM1_UP_IRQHandler(void)
{
	static uint16_t Count;
	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		/*定时中断函数1ms自动执行一次*/
		Count++;
		Key_Tick();		//调用按键模块的Tick函数，用于驱动按键模块工作
		
		if(Count>=40){
			Count=0; 
			
			Actual=Encoder_Get();
			Error1=Error0;
			Error0=Target-Actual;
			
			ErrorIntOut+=Ki*Error0;
			//100为PWA中ARR决定
			if(ErrorIntOut>100)ErrorIntOut=100;
			if(ErrorIntOut<-100.0)ErrorIntOut=100;
			
			Out=Kp*Error0+ErrorIntOut+Kd*(Error0-Error1);
			
			if(Out>100)Out=100;
			if(Out<-100)Out=-100;
			
			if(MotorFlag){
				Motor_SetPWM(Out);
			}else{
				Motor_SetPWM(0);
			}
		}
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}
