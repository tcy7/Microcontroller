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


int16_t PWM;
uint16_t RP1,RP2,RP3,RP4;

int main(void)
{
	OLED_Init();
	AD_Init();
	
	while (1)
	{
		OLED_Printf(0, 0, OLED_8X16, "AD:%04d", AD_GetValue());
		OLED_Update();		
	} 
}
