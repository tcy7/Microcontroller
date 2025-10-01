#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "MyDMA.h"
#include "Delay.h"

uint8_t DataA[]={0x01,0x02,0x03,0x04};
uint8_t DataB[4];

int main(void){
	OLED_Init();
	MyDMA_Init((uint32_t)DataA,(uint32_t)DataB,4);
	
	
	
	Delay_ms(1000);
	
	while (1){
		DataA[0]++;
		DataA[1]++;
		DataA[2]++;
		DataA[3]++;
		OLED_ShowString(1,1,"DataA");
		OLED_ShowString(3,1,"DataB");
		OLED_ShowHexNum(1,7,(uint32_t)DataA,8);
		OLED_ShowHexNum(3,7,(uint32_t)DataB,8);
		
		OLED_ShowHexNum(2,1,DataA[0],2);
		OLED_ShowHexNum(2,4,DataA[1],2);
		OLED_ShowHexNum(2,7,DataA[2],2);
		OLED_ShowHexNum(2,10,DataA[3],2);	
		OLED_ShowHexNum(4,1,DataB[0],2);
		OLED_ShowHexNum(4,4,DataB[1],2);
		OLED_ShowHexNum(4,7,DataB[2],2);
		OLED_ShowHexNum(4,10,DataB[3],2);
		MyDMA_Transfer();
		Delay_ms(1000);
		
		OLED_ShowHexNum(2,1,DataA[0],2);
		OLED_ShowHexNum(2,4,DataA[1],2);
		OLED_ShowHexNum(2,7,DataA[2],2);
		OLED_ShowHexNum(2,10,DataA[3],2);	
		OLED_ShowHexNum(4,1,DataB[0],2);
		OLED_ShowHexNum(4,4,DataB[1],2);
		OLED_ShowHexNum(4,7,DataB[2],2);
		OLED_ShowHexNum(4,10,DataB[3],2);
	}
}
