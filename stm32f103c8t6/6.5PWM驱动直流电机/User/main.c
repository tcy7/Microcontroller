#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Motor.h"
#include "Delay.h"
#include "Key.h"
uint16_t KeyNum,Speed;

int main(void){
	OLED_Init();
	Motor_Init();
	Key_Init();
	
	while (1){
		KeyNum=Key_GetNum();
		if(KeyNum==1){
			Speed+=20;
			if(Speed>100){
				Speed=0;               
			}
			Motor_SetSpeed(Speed);
		}
		OLED_ShowSignedNum(1, 7, Speed, 3);
	}
}
