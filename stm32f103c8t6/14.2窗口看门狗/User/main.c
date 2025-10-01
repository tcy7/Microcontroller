#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Key.h"
#include "Delay.h"

int main(void){
	OLED_Init();
	Key_Init();
	
	OLED_ShowString(1,1,"WWDG_TEST");
	
	if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST)==SET){
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
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);	//开启WWDG的时钟
	
	WWDG_SetPrescaler(WWDG_Prescaler_8);			//设置预分频为8
	WWDG_SetWindowValue(0x40 | 21);					//设置窗口值，窗口时间为30ms
	WWDG_Enable(0x40 | 54);	
	
	while (1){
		Key_GetNum();
		
		
		OLED_ShowString(4,1,"FEED");
		
		OLED_ShowString(4,1,"FEED");
		Delay_ms(500);
		OLED_ShowString(4,1,"       ");
		Delay_ms(500);
		WWDG_SetCounter(0x40|54);

	}
}
