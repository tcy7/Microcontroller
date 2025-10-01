#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Serial.h"
#include "Delay.h"

uint8_t RxData;

int main(void){
	OLED_Init();
	Serial_Init();
	OLED_ShowString(1, 1, "RxData:");
	
	while (1){
		if(Serial_GetRxFlag()==1){
			RxData=Serial_GetRxData();
			Serial_SendByte(RxData);
			OLED_ShowHexNum(1,8,RxData ,2);
		}
		
		OLED_ShowString(2,1,"running");
		Delay_ms(500);
		OLED_ShowString(2,1,"       ");
		Delay_ms(500);	
	
		__WFI();
	}
}
