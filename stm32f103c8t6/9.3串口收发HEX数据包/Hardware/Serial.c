#include "stm32f10x.h"                  // Device header
#include "stdio.h"
#include "stdarg.h"

uint8_t Serial_TxPacket[4];
uint8_t Serial_RxPacket[4];
uint8_t Serial_RxFlag;


void Serial_Init(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;//单片机输出口
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate=9600;
	USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;
	USART_InitStruct.USART_Parity=USART_Parity_No;
	USART_InitStruct.USART_StopBits=USART_StopBits_1;
	USART_InitStruct.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART1,&USART_InitStruct);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);	
	USART_Cmd(USART1, ENABLE);
}

void Serial_SendByte(uint8_t Byte){
	USART_SendData(USART1,Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	//下次写入数据寄存器会自动清除发送完成标志位，故此循环后，无需清除标志位
}

void Serial_SendArray(uint8_t *Array,uint16_t Length){
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}
	
void Serial_SendString(uint8_t *String){
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)//遍历字符数组（字符串），遇到字符串结束标志位后停止
	{
		Serial_SendByte(String[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y){
	uint32_t Result = 1;	//设置结果初值为1
	while (Y --)			//执行Y次
	{
		Result *= X;		//将X累乘到结果
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length){
	uint8_t i;
	for(i=0;i<Length;i++){
		Serial_SendByte(Number/Serial_Pow(10,Length-1-i)%10+'0');
	}
}

int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);			//将printf的底层重定向到自己的发送字节函数
	return ch;
}

//void Serial_Printf(char *format, ...)
//{
//	char String[100];				//定义字符数组
//	va_list arg;					//定义可变参数列表数据类型的变量arg
//	va_start(arg, format);			//从format开始，接收参数列表到arg变量
//	vsprintf(String, format, arg);	//使用vsprintf打印格式化字符串和参数列表到字符数组中
//	va_end(arg);					//结束变量arg
//	Serial_SendString(String);		//串口发送字符数组（字符串）
//}

void Serial_SendPacket(void){
	Serial_SendByte(0xFF);
	Serial_SendArray(Serial_TxPacket,4);
	Serial_SendByte(0xFE);
}

uint8_t Serial_GetRxFlag(){
	if(Serial_RxFlag){
		Serial_RxFlag=0;
		return 1;
	}
	return 0;
}

void USART1_IRQHandler(void){
	
	static uint8_t ReState;
	static uint8_t pRePacket;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET){
		//寄存器非空转空的瞬间进入中断
		uint8_t RxData=USART_ReceiveData(USART1);
		if(ReState==0){
			if(RxData==0xFF){
				ReState=1;
				pRePacket=0;
			}
		}else if(ReState==1){
			Serial_RxPacket[pRePacket]=RxData;
			pRePacket++;
			if(pRePacket==4){
				ReState=2;
			}
		}else if(ReState==2){
			if(RxData==0xFE){
				ReState=0;
				Serial_RxFlag=1;//单片机成功接收到一个数据包
			}
		}
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}
