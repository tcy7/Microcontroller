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


//通过按键先调内环再调外环确定双环pid值
float InnerActual,InnerTarget,InnerOut;
float InnerKp,InnerKi,InnerKd;
float InnerError0,InnerError1,InnerErrorInt;

float OuterActual,OuterTarget,OuterOut;
float OuterKp,OuterKi,OuterKd;
float OuterError0,OuterError1,OuterErrorInt;

uint8_t KeyNum;
uint16_t Speed,Location;

int main(void)
{
	OLED_Init();
	Key_Init();
	Motor_Init();	
	Encoder_Init();
	Timer_Init();
	Serial_Init();
	
	OLED_Printf(0, 0, OLED_8X16, "2*PID Control");
	OLED_Update();
	
	while (1)
	{
		/*按键修改目标值外环时使用OuterTarget
			解除注释后使用按键调值关闭电位器旋钮
		*/
//		KeyNum = Key_GetNum();	
//		if (KeyNum == 1)			
//		{
//			InnerTarget += 10;		
//			if (InnerTarget > 100) {InnerTarget = 100;}	
//		}
//		if (KeyNum == 2)		
//		{
//			InnerTarget -= 10;		
//			if (InnerTarget < -100) {InnerTarget = -100;}	
//		}
//		if (KeyNum == 3)		
//		{
//			InnerTarget = 0;		
//		}

		/*内环PID调参，测试出PID内环值，此时注释外环调参代码（外环调参时不需要注释内环）
		*/
//		
//		InnerKp=RP_GetValue(1)/4095*2;
//		InnerKi=RP_GetValue(2)/4095*2;
//		InnerKd=RP_GetValue(3)/4095*2;
//		InnerTarget=RP_GetValue(4)/ 4095.0 * 300-150;

//		OLED_Printf(0,16, OLED_8X16, "Kp:%4.2f", InnerKp);		
//		OLED_Printf(0,32, OLED_8X16, "Ki:%4.2f", InnerKi);		
//		OLED_Printf(0,48, OLED_8X16, "Kd:%4.2f", InnerKd);
//		OLED_Printf(64,16, OLED_8X16, "Tar:%+04.0f", InnerTarget);	
//		OLED_Printf(64,32, OLED_8X16, "Act:%+04.0f", InnerActual);	
//		OLED_Printf(64,48, OLED_8X16, "Out:%+04.0f", InnerOut);	
//
//		OLED_Update();
//
//		Serial_Printf("%f,%f,%f\r\n", InnerTarget, InnerActual, InnerOut);
//		



//		OuterKp=RP_GetValue(1)/4095*2;
//		OuterKi=RP_GetValue(2)/4095*2;
//		OuterKd=RP_GetValue(3)/4095*2;
		OuterTarget=RP_GetValue(4)/ 4095.0 * 816-408;//该型号编码器旋转一周的计数是408
		
		OLED_Printf(0, 16, OLED_8X16, "Kp:%4.2f", OuterKp);		
		OLED_Printf(0, 32, OLED_8X16, "Ki:%4.2f", OuterKi);		
		OLED_Printf(0, 48, OLED_8X16, "Kd:%4.2f", OuterKd);		
		OLED_Printf(64, 16, OLED_8X16, "Tar:%+04.0f", OuterTarget);	
		OLED_Printf(64, 32, OLED_8X16, "Act:%+04.0f", OuterActual);	
		OLED_Printf(64, 48, OLED_8X16, "Out:%+04.0f", OuterOut);		
		
		OLED_Update();
	
		Serial_Printf("%f,%f,%f\r\n",OuterTarget,OuterActual,OuterOut);
	}
}

void TIM1_UP_IRQHandler(void)
{
	static uint16_t Count1,Count2;
	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		/*定时中断函数1ms自动执行一次*/
		Key_Tick();		//调用按键模块的Tick函数，用于驱动按键模块工作
		
		//内环计次分频
		Count1++;
		if(Count1>=40){
			Count1=0; 
			
			Speed=Encoder_Get();
			Location+=Speed;
			
			InnerActual=Speed;
			InnerError1=InnerError0;
			InnerError0=InnerTarget-InnerActual;
			
			if(fabs(InnerKi)>EPSILON){
				InnerErrorInt+=InnerError0;
			}else{
				InnerErrorInt=0;
			}
			
			InnerOut=InnerKp*InnerError0+InnerKi*InnerErrorInt+InnerKd*(InnerError0-InnerError1);
			if(InnerOut>100)InnerOut=100;
			if(InnerOut<-100)InnerOut=-100;
			
			Motor_SetPWM(InnerOut);
		}
		
		Count2++;
		if(Count2>=40){
			Count2=0;
			
			OuterActual=Location;
			
			OuterError1=OuterError0;
			OuterError0=OuterTarget-OuterActual;
			
			if(fabs(InnerKi)>EPSILON){
				InnerErrorInt+=InnerError0;
			}else{
				InnerErrorInt=0;
			}

			OuterOut=OuterKp * OuterError0 + OuterKi * OuterErrorInt + OuterKd * (OuterError0 - OuterError1);
			
			if(OuterOut>20)OuterOut=20;
			if(OuterOut<-20)OuterOut=-20;
			
			InnerTarget=OuterOut;
		}
		
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}
