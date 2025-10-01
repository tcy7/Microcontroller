#include "stm32f10x.h"                  // Device header
#include <time.h>
#include "MyRTC.h"

uint16_t MyRTC_Time[]={2025,9,21,12,53,1};


//void MyRTC_Init(){
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP,ENABLE);
//	PWR_BackupAccessCmd(ENABLE);//使能备份寄存器的访问，使用PWR开启对备份寄存器的访问
//	
//	//选择外部低速时钟LSE，等待标志位确定时钟已开启
//	RCC_LSEConfig(RCC_LSE_ON);
//	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET);
//	
//	//选择通道，确定LSE时钟频率进入分频器
//	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
//	RCC_RTCCLKCmd(ENABLE);
//	
//	//等待同步，等待上一次写入操作完成
//	RTC_WaitForSynchro();
//	RTC_WaitForLastTask();
//	/*
//	必须设置RTC_CRL寄存器中的CNF位，使RTC进入配置模式后才能写入RTC_PRL,RTC_CNT,RTC_ALR寄存器
//	但在标准库函数中已经封装进入配置模式退出配置模式，所以下面对RTC的读写不写进入配置模式的代码
//	*/
//	//设置PRL分频
//	RTC_SetPrescaler(32768-1);
//	RTC_WaitForLastTask();
//	
////	//设置计数器值
////	RTC_SetCounter(1672588795);
////	RTC_WaitForLastTask();
//	MyRTC_SetTime();
//}

void MyRTC_Init(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP,ENABLE);
	PWR_BackupAccessCmd(ENABLE);//使能备份寄存器的访问，使用PWR开启对备份寄存器的访问
	
	if(BKP_ReadBackupRegister(BKP_DR1)!=0xA5A5){
	//选择外部低速时钟LSE，等待标志位确定时钟已开启
	RCC_LSEConfig(RCC_LSE_ON);
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET);
	
	//选择通道，确定LSE时钟频率进入分频器
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);
	
	//等待同步，等待上一次写入操作完成
	RTC_WaitForSynchro();
	RTC_WaitForLastTask();
	/*
	必须设置RTC_CRL寄存器中的CNF位，使RTC进入配置模式后才能写入RTC_PRL,RTC_CNT,RTC_ALR寄存器
	但在标准库函数中已经封装进入配置模式退出配置模式，所以下面对RTC的读写不写进入配置模式的代码
	*/
	//设置PRL分频
	RTC_SetPrescaler(32768-1);
	RTC_WaitForLastTask();
	
//	//设置计数器值
//	RTC_SetCounter(1672588795);
//	RTC_WaitForLastTask();
	MyRTC_SetTime();
	BKP_WriteBackupRegister(BKP_DR1,0xA5A5);
	
	}else{
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
	}
}


void MyRTC_SetTime(){
	time_t time_cnt;
	struct tm time_date;
	
	time_date.tm_year=MyRTC_Time[0]-1900;
	time_date.tm_mon=MyRTC_Time[1]-1;
	time_date.tm_mday=MyRTC_Time[2];
	time_date.tm_hour=MyRTC_Time[3];
	time_date.tm_min=MyRTC_Time[4];
	time_date.tm_sec=MyRTC_Time[5];
	
	time_cnt=mktime(&time_date);
	
	RTC_SetCounter(time_cnt);
	RTC_WaitForLastTask();
	
}

void MyRTC_ReadTime(){
	time_t time_cnt;
	struct tm time_date;
	
	time_cnt=RTC_GetCounter();
	time_date = *localtime(&time_cnt);
	
	MyRTC_Time[0] = time_date.tm_year + 1900;
	MyRTC_Time[1] = time_date.tm_mon + 1;
	MyRTC_Time[2] = time_date.tm_mday;
	MyRTC_Time[3] = time_date.tm_hour;
	MyRTC_Time[4] = time_date.tm_min;
	MyRTC_Time[5] = time_date.tm_sec;	
	
}


