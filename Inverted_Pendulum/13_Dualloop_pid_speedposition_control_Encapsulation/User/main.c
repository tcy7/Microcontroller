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
#include "PID.h"

#define EPSILON 0.0001

uint8_t KeyNum;
uint16_t Speed,Location;

PID_t Inner={
	.Kp=0.3,.Ki=0.3,.Kd=0,
	.OutMax=100,.OutMin=-100,
};

PID_t Outer={
	.Kp=0.4,.Ki=0,.Kd=0.4,
	.OutMax=20,.OutMin=-20,
};

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

		Outer.Target=RP_GetValue(4)/ 4095.0 * 816-408;//该型号编码器旋转一周的计数是408
		
		OLED_Printf(0, 16, OLED_8X16, "Kp:%4.2f", Outer.Kp);		
		OLED_Printf(0, 32, OLED_8X16, "Ki:%4.2f", Outer.Ki);		
		OLED_Printf(0, 48, OLED_8X16, "Kd:%4.2f", Outer.Kd);		
		OLED_Printf(64, 16, OLED_8X16, "Tar:%+04.0f", Outer.Target);	
		OLED_Printf(64, 32, OLED_8X16, "Act:%+04.0f", Outer.Actual);	
		OLED_Printf(64, 48, OLED_8X16, "Out:%+04.0f", Outer.Out);		
		
		OLED_Update();
	
		Serial_Printf("%f,%f,%f\r\n",Outer.Target,Outer.Actual,Outer.Out);
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
			
			Inner.Actual=Speed;
			PID_Update(&Inner);
			
			Motor_SetPWM(Inner.Out);
		}
		
		Count2++;
		if(Count2>=40){
			Count2=0;
			
			Outer.Actual=Location;
			PID_Update(&Outer);
			
			Inner.Target=Outer.Out;
		}
		
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}
