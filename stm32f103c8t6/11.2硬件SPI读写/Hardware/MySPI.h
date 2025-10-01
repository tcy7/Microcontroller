#ifndef __MYSPI_H__
#define __MYSPI_H__

void MySPI_W_SS(uint8_t BitVal);
void MySPI_W_SCK(uint8_t BitVal);
void MySPI_W_MOSI(uint8_t BitVal);
uint8_t MySPI_R_MISO(void);
void MySPI_Init(void);
void MySPI_Start(void);
void MySPI_Stop(void);
uint8_t MySPI_SwapByte(uint8_t ByteSend);

#endif
