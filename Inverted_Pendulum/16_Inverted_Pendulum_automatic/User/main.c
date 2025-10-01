#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "LED.h"
#include "Timer.h"
#include "Key.h"
#include "RP.h"
#include "Motor.h"
#include "Serial.h"
#include "AD.h"
#include "Encoder.h"
#include "PID.h"

/*
	中心角度值视设备而定一般在1900~2200之间
	中心角度值不确定会导致摆杆总往一个方向跑且位置环PD控制器有静态误差
	及最终按键控制横杆正转一圈和反转一圈的速度不同
*/
#define CENTER_ANGLE 2010
#define CENTER_RANGE 500

#define START_PWM 35
#define START_TIME 100


uint8_t KeyNum;
uint8_t RunState;

uint16_t Angle;
int16_t Speed, Location;

PID_t AnglePID={
	.Target = CENTER_ANGLE,
	
	.Kp = 0.3,.Ki = 0.01,.Kd = 0.4,

	.OutMax = 100,.OutMin = -100,
};

PID_t LocationPID={
	.Target = 0,
	
	.Kp = 0.3,.Ki = 0,.Kd = 4,

	.OutMax = 100,.OutMin = -100,
};

int main(void)
{
	OLED_Init();
	LED_Init();
	Key_Init();
	RP_Init();
	Motor_Init();
	Encoder_Init();
	Serial_Init();
	AD_Init();
	
	Timer_Init();
	
	while (1)
	{
		KeyNum=Key_GetNum();
		if(KeyNum==1){
			if (RunState == 0){
				RunState = 21;
			}else{
				RunState = 0;
			}
		}
		if(KeyNum==2){
			LocationPID.Target += 408;		
			if (LocationPID.Target > 4080)
			{
				LocationPID.Target = 4080;	
			}
		}
		if(KeyNum==3){
			LocationPID.Target -= 408;		
			if (LocationPID.Target < -4080)	
			{
				LocationPID.Target = -4080;	
			}
		}
		
		if(RunState){
			LED_ON();
		}else{
			LED_OFF();
		}
		
//		AnglePID.Kp = RP_GetValue(1) / 4095.0 * 1;
//		AnglePID.Ki = RP_GetValue(2) / 4095.0 * 1;
//		AnglePID.Kd = RP_GetValue(3) / 4095.0 * 1;
//		
		OLED_Printf(42, 0, OLED_6X8, "%02d", RunState);			//显示运行状态，便于调试
		
		OLED_Printf(0, 0, OLED_6X8, "Angle");							
		OLED_Printf(0, 12, OLED_6X8, "Kp:%05.3f", AnglePID.Kp);			
		OLED_Printf(0, 20, OLED_6X8, "Ki:%05.3f", AnglePID.Ki);			
		OLED_Printf(0, 28, OLED_6X8, "Kd:%05.3f", AnglePID.Kd);			
		OLED_Printf(0, 40, OLED_6X8, "Tar:%04.0f", AnglePID.Target);	
		OLED_Printf(0, 48, OLED_6X8, "Act:%04d", Angle);	//显示实际值，使用Angle而不是AnglePID.Actual，可以实现PID程序停止运行时仍然刷新实际值
		OLED_Printf(0, 56, OLED_6X8, "Out:%+04.0f", AnglePID.Out);
		OLED_Update();
		
//		LocationPID.Kp = RP_GetValue(1) / 4095.0 * 1;
//		LocationPID.Ki = RP_GetValue(2) / 4095.0 * 1;
//		LocationPID.Kd = RP_GetValue(3) / 4095.0 * 10;
//		
		OLED_Printf(64, 0, OLED_6X8, "Location");							
		OLED_Printf(64, 12, OLED_6X8, "Kp:%05.3f", LocationPID.Kp);			
		OLED_Printf(64, 20, OLED_6X8, "Ki:%05.3f", LocationPID.Ki);			
		OLED_Printf(64, 28, OLED_6X8, "Kd:%05.3f", LocationPID.Kd);			
		OLED_Printf(64, 40, OLED_6X8, "Tar:%+05.0f", LocationPID.Target);	
		OLED_Printf(64, 48, OLED_6X8, "Act:%+05d", Location);
		OLED_Printf(64, 56, OLED_6X8, "Out:%+04.0f", LocationPID.Out);
		OLED_Update();

	} 
}

void TIM1_UP_IRQHandler(void)
{
	/*
		该中断1ms进入一次
		执行以下中断程序约20us远低于1ms不会影响程序正常运行
	*/
	static uint16_t Count,Count1,Count2,CountTime;
	//分别为1中判断计次，角度环计次，位置环计次，延时计次
	static uint16_t Angle0, Angle1, Angle2;//本次角度、上次角度、上上次角度，用于判断摆杆处于最高点位置

	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		
		Key_Tick();
		
		Angle=AD_GetValue();
		Speed = Encoder_Get();
		Location += Speed;
		
		//停止状态
		if(RunState==0)
		{
			Motor_SetPWM(0);
		}
		//判断状态，一个起摆过程结束进入该状态判断是否进入摆动区
		else if(RunState==1)
		{
			Count++;
			if(Count>= 40){
				Count=0;
				Angle2 = Angle1;
				Angle1 = Angle0;
				Angle0 = Angle;
				
				/*判断摆杆当前是否处于右侧最高点*/
				if (Angle0 > CENTER_ANGLE + CENTER_RANGE		//3次角度值均位于右侧区间
				 && Angle1 > CENTER_ANGLE + CENTER_RANGE
				 && Angle2 > CENTER_ANGLE + CENTER_RANGE
				 && Angle1 < Angle0								//且中间一次角度值是最小的，表示摆杆处于右侧最高点
				 && Angle1 < Angle2)
				{
					RunState = 21;	//状态转入21，执行向左施加瞬时驱动力的程序
				}
				/*判断摆杆当前是否处于左侧最高点*/
				if (Angle0 < CENTER_ANGLE - CENTER_RANGE		//3次角度值均位于左侧区间
				 && Angle1 < CENTER_ANGLE - CENTER_RANGE
				 && Angle2 < CENTER_ANGLE - CENTER_RANGE
				 && Angle1 > Angle0								//且中间一次角度值是最大的，表示摆杆处于左侧最高点
				 && Angle1 > Angle2)
				{
					RunState = 31;				//状态转入31，执行向右施加瞬时驱动力的程序
				}
				
				/*判断摆杆当前是否进入中心区间*/
				if (Angle0 > CENTER_ANGLE - CENTER_RANGE		//2次角度值均位于中心区间
				 && Angle0 < CENTER_ANGLE + CENTER_RANGE
				 && Angle1 > CENTER_ANGLE - CENTER_RANGE
				 && Angle1 < CENTER_ANGLE + CENTER_RANGE)
				{
					/*一些变量初始化归零，避免错误的初值影响PID程序运行*/
					/*将起摆成功位置定为0位置防止起摆后转圈*/
					Location = 0;
					/*施加外力长时间积累误差积分使设备退出倒立摆状态后再次起摆会因为原来的饱和积分导致无法起摆*/
					AnglePID.ErrorInt = 0;		//角度环误差积分归零
					LocationPID.ErrorInt = 0;	//位置环误差积分归零
					
					/*启摆完成*/
					RunState = 4;				//状态转入4，执行PID控制程序
				}
			}
		}
		
		//起摆状态
		else if(RunState==21)
		{
			Motor_SetPWM(START_PWM);
			CountTime = START_TIME;
			RunState = 22;
		}
		else if(RunState==22)//计数延时作用
		{
			CountTime --;
			if (CountTime == 0)			//如果减到0了，说明计时时间到
			{
				RunState = 23;			//随后状态转入23
			}		
		}
		else if(RunState==23)
		{
			Motor_SetPWM(-START_PWM);
			CountTime = START_TIME;
			RunState = 24;
		}
		else if(RunState==24)
		{
			CountTime --;
			if (CountTime == 0)
			{
				Motor_SetPWM(0);		//向左施加瞬时驱动力过程结束，电机停止
				RunState=1;
			}		
		}
		
		else if(RunState==31)
		{
			Motor_SetPWM(-START_PWM);
			CountTime = START_TIME;
			RunState =32;
		}
		else if(RunState==32)
		{
			CountTime --;
			if (CountTime == 0)
			{
				RunState=1;
			}		
		}
		else if(RunState==33)
		{
			Motor_SetPWM(START_PWM);
			CountTime = START_TIME;
			RunState =34;
		}
		else if(RunState==34)
		{
			CountTime --;
			if (CountTime == 0)
			{
				Motor_SetPWM(0);		//向右施加瞬时驱动力过程结束，电机停止
				RunState=1;
			}		
		}
		
		//倒立摆状态
		else if(RunState==4)
		{
			//判断是否从倒立摆状态倒下，角度超过规定区间则停止PID程序
			if(!(Angle>CENTER_ANGLE-CENTER_RANGE)
				&&(Angle<CENTER_ANGLE+CENTER_RANGE))RunState=0;
			
				//角度环分频计次
				Count1++;
				if(Count1>=5){
					Count1=0;
					
					AnglePID.Actual = Angle;
					PID_Update(&AnglePID);
					Motor_SetPWM(AnglePID.Out);
				}
				
				//位置环分频计次
				Count2++;
				if(Count2>=50){
					Count2=0;
					
					LocationPID.Actual = Location;	
					PID_Update(&LocationPID);
					AnglePID.Target = CENTER_ANGLE-LocationPID.Out;
				}
		}
		
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}
