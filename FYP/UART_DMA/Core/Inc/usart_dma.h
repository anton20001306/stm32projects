/*
 * usart_interrupt.h
 *
 *  Created on: Apr 10, 2026
 *      Author: Praveen
 */

#ifndef INC_USART_DMA_H_
#define INC_USART_DMA_H_

#include "main.h"

#define RXSIZE 20
#define MSIZE  50

extern volatile uint8_t RxBuf[RXSIZE];
extern volatile uint8_t MainBuf [MSIZE];


void USART_Init(void);
void DMA_Rx_Init(void);
void DMA_TX_Init(void);
void DMA_TX_Config(uint8_t *data, uint16_t len);
void DMA_Rx_Config (uint32_t srcAdd, uint32_t destAdd, uint16_t datasize);
void DMA1_Stream5_IRQHandler(void);

void Tim6_Tick(void);


#endif /* INC_USART_DMA_H_ */
