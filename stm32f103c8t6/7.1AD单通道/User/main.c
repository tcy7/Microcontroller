#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "AD.h"

float Voltage;
uint16_t ADValue;

int main(void){
	OLED_Init();
	AD_Init();
	
	OLED_ShowString(1, 1, "ADValue:");
	OLED_ShowString(2, 1, "Voltage:0.00V");
	
	while (1){
		OLED_ShowNum(1,9,AD_GetValue(),4);
		
		ADValue = AD_GetValue();
		Voltage = (float)ADValue / 4095 * 3.3;
		OLED_ShowNum(2,9,Voltage,1);
		OLED_ShowNum(2,11,(uint16_t)(Voltage*100)%100,2);
	}
}
