#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "IC.h"
#include "PWM.h"


int main(void){
	OLED_Init();
	IC_Init();
	PWM_Init();
	
	OLED_ShowString(1, 1, "Freq:00000Hz");
	PWM_SetPrescaler(720-1);//源为1kHz
	PWM_SetCompare1(50);
	
	while (1){
		OLED_ShowNum(1, 6, IC_GetFreq(), 5);
	}
}
