#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Key.h"
#include "Delay.h"

int main(void){
	OLED_Init();
	Key_Init();
	
	OLED_ShowString(1,1,"IWDG_TEST");
	
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST)==SET){
		OLED_ShowString(2,1,"IWDGRST");
		Delay_ms(500);
		OLED_ShowString(2,1,"       ");
		Delay_ms(500);
		RCC_ClearFlag();
	}else{
		OLED_ShowString(3,1,"RST");
		Delay_ms(500);
		OLED_ShowString(3,1,"       ");
		Delay_ms(500);

	}
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	//独立看门狗写使能
	IWDG_SetPrescaler(IWDG_Prescaler_16);			//设置预分频为16
	IWDG_SetReload(2499);							//设置重装值为2499，独立看门狗的超时时间为1000ms
	IWDG_ReloadCounter();							//重装计数器，喂狗
	IWDG_Enable();
	
	while (1){
		Key_GetNum();
		
		IWDG_ReloadCounter();
		
		OLED_ShowString(4,1,"FEED");
		
		OLED_ShowString(4,1,"FEED");
		Delay_ms(500);
		OLED_ShowString(4,1,"       ");
		Delay_ms(500);

	}
}
