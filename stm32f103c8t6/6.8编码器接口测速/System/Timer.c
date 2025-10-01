#include "stm32f10x.h"                  // Device header

//定时中断初始化
void Timer_Init(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//开启时钟TIM2
	
	TIM_InternalClockConfig(TIM2);//配置时钟源，设置TIM2使用内部时钟，此行可以省略默认时也是使用内部时钟
	//时基单元初始化
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period=10000-1;
	TIM_TimeBaseInitStruct.TIM_Prescaler=7200-1;
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);	
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);//清除定时器标志位
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);//开启TIM2的更新中断
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//配置NVIC分组
	NVIC_InitTypeDef NVIC_InitStruct;//定义结构体变量
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM2,ENABLE);//计数器使能
	
}

