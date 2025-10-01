#include "stm32f10x.h"                  // Device header
#include "CountSensor.h"
#include "OLED.h"
#include "Delay.h"

int main(void){
	
	CountSensor_Init();
	OLED_Init();
	
	OLED_ShowString(1, 1, "Count:");
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	while(1){
		
 		OLED_ShowNum(1,7,CountSensor_Get(),5);
		
		OLED_ShowString(2,1,"running");
		Delay_ms(500);
		OLED_ShowString(2,1,"       ");
		Delay_ms(500);
		
		PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);	//STM32进入停止模式，并等待中断唤醒
		SystemInit();										//唤醒后，要重新配置时钟

	}
	
}
