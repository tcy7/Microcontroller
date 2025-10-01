#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Store.h"
#include "Key.h"

uint8_t KeyNum;
int main(void){
	OLED_Init();
	Key_Init();
	
	
	while (1){
		KeyNum=Key_GetNum();
		
		if(KeyNum==1){
			
		}
		if(KeyNum==2){
			
		}
	}
}
