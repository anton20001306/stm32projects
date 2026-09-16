/*
 * usart_interrupt.c
 *
 *  Created on: Apr 10, 2026
 *      Author: Praveen
 */
#include "stm32f446xx.h"

#include "usart_dma.h"
#include "string.h"
#include "adc.h"

#define RXSIZE 20
#define MSIZE  50

volatile uint8_t RxBuf[RXSIZE];
volatile uint8_t MainBuf[MSIZE];
volatile uint8_t txFrame[8];

volatile uint8_t indx;

volatile uint8_t button_state = 1;
volatile uint8_t keypad_val = 2;

void USART_Init(void)
{
	/*RCC clock enable*/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;		// USART2  enable

	/*Configure USART pins PA2 and PA3*/
	GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
	GPIOA->MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1);

	/*Set Alternate function mode*/
	GPIOA->AFR[0] &= ~(0xFF << (4 * 2));
	GPIOA->AFR[0] |=  (0x77 << (4 * 2));

	/*Set high speed*/
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDER_OSPEEDR3);
	GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDER_OSPEEDR3);

	/*USART2 initialization*/
	USART2->CR1 = 0;							// Reset the CR1

	USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;	// Enable transmission and receive

	/*Define 8 bit data length*/
	USART2->CR1 &= ~(USART_CR1_M);

	/*Define the baud rate*/
	USART2->BRR = (22 << 4) | (13);   // 115200 baud @ PCLK1 = 42 MHz

	USART2->CR1 |= USART_CR1_UE;				// Enable USART2

	/************************************* USART_DMA ********************************************/

	USART2->CR3 |= USART_CR3_DMAT;				// Enable DMA for Transmit
	USART2->CR3 |= USART_CR3_DMAR;				// Enable DMA for Receive

}

void DMA_TX_Init(void)
{
	// DMA1 clock already enabled in DMA_Init(), but harmless to repeat
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

	// Make sure stream is off before configuring
	DMA1_Stream6->CR &= ~(1 << 0);
	while (DMA1_Stream6->CR & (1 << 0));

	// Channel 4 = USART2_TX
	DMA1_Stream6->CR &= ~(0x7 << 25);
	DMA1_Stream6->CR |=  (0x4 << 25);

	// Direction: memory-to-peripheral (opposite of RX)
	DMA1_Stream6->CR &= ~(0x3 << 6);
	DMA1_Stream6->CR |=  (0x1 << 6);           // DIR = 01

	// Circular OFF — one-shot per frame
	DMA1_Stream6->CR &= ~(0x1 << 8);

	// Memory increment ON (step through the frame bytes)
	DMA1_Stream6->CR |= (0x1 << 10);

	// 8-bit peripheral and memory data size
	DMA1_Stream6->CR &= ~(0x3 << 11);          // PSIZE = 00
	DMA1_Stream6->CR &= ~(0x3 << 13);          // MSIZE = 00

	// Priority low
	DMA1_Stream6->CR &= ~(0x3 << 16);

	// Peripheral address is fixed — set once here
	DMA1_Stream6->PAR = (uint32_t)&USART2->DR;
}

void DMA_Rx_Init(void)
{


	// 1. Enable DMA1 clock
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;        // NOT AHBENR — F4 uses AHB1ENR

	// Make sure the stream is off before configuring (required on F4)
//	DMA1_Stream5->CR &= ~DMA_SxCR_EN;
//	while (DMA1_Stream5->CR & DMA_SxCR_EN);    // wait until it truly disables


	// 2. Select Channel 4 for this stream (the USART2_RX mapping)
	DMA1_Stream5->CR &= ~(0x7 << 25);          // clear channel selector
	DMA1_Stream5->CR |=  (0x4 << 25);          // channel 4

	// 3. Data direction: peripheral-to-memory (RX = read from USART)
	DMA1_Stream5->CR &= ~(0x3 << 6);           // DIR = 00 : peripheral-to-memory

	// 4. Circular mode
	DMA1_Stream5->CR |= (0x1 << 8);            // CIRC = 1

	// 5. Memory increment
	DMA1_Stream5->CR |= (0x1 << 10);           // MINC = 1

	// 6. Peripheral data size = 8-bit
	DMA1_Stream5->CR &= ~(0x3 << 11);          // PSIZE = 00

	// 7. Memory data size = 8-bit
	DMA1_Stream5->CR &= ~(0x3 << 13);          // MSIZE = 00

	// 8. Priority level
	DMA1_Stream5->CR &= ~(0x3 << 16);          // PL = 00 (low)

	// 9. Transfer-complete / half / error interrupts
	DMA1_Stream5->CR |= (0x1 << 4)             // TCIE
	                 |  (0x1 << 3)             // HTIE
	                 |  (0x1 << 2);            // TEIE
}

void DMA_Rx_Config (uint32_t srcAdd, uint32_t destAdd, uint16_t datasize)
{
	// 1. Set the data size (number of transfers)
	DMA1_Stream5->NDTR = datasize;   // was CNDTR

	// 2. Set the peripheral address (source = USART2->DR)
	DMA1_Stream5->PAR = srcAdd;      // was CPAR

	// 3. Set the memory address (destination = your buffer)
	DMA1_Stream5->M0AR = destAdd;    // was CMAR

	// 4. Enable the DMA stream (EN = bit 0)
	DMA1_Stream5->CR |= (1<<0);      // was CCR; DMA_SxCR_EN
}

void DMA1_Stream5_IRQHandler(void)
  {
      if (DMA1->HISR & DMA_HISR_HTIF5) {                 // half transfer, stream 5
          memcpy((void*)&MainBuf[indx], (const void*)&RxBuf[0], RXSIZE/2);
          DMA1->HIFCR |= DMA_HIFCR_CHTIF5;               // clear HT flag
          indx += RXSIZE/2;
          if (indx > 49) indx = 0;
      }
      if (DMA1->HISR & DMA_HISR_TCIF5) {                 // transfer complete, stream 5
          memcpy((void*)&MainBuf[indx], (const void*)&RxBuf[RXSIZE/2], RXSIZE/2);
          DMA1->HIFCR |= DMA_HIFCR_CTCIF5;               // clear TC flag
          indx += RXSIZE/2;
          if (indx > 49) indx = 0;
      }
  }

void DMA_TX_Send(uint8_t *data, uint16_t len)
{
	// Must be disabled to change M0AR/NDTR
	DMA1_Stream6->CR &= ~(1 << 0);
	while (DMA1_Stream6->CR & (1 << 0));

	// Clear all Stream6 flags (bits 21..26 in HIFCR) before re-arming
	DMA1->HIFCR = (0x3F << 21);

	DMA1_Stream6->M0AR = (uint32_t)data;
	DMA1_Stream6->NDTR = len;

	DMA1_Stream6->CR |= (1 << 0);   // fire
}

void Tim6_Tick(void)
{
	txFrame[0] = 0xAA;
	txFrame[1] = adcData[0] >> 8;      // joystick X hi  (PA1, ch1)
	txFrame[2] = adcData[0] & 0xFF;
	txFrame[3] = adcData[1] >> 8;      // joystick Y hi  (PA4, ch4)
	txFrame[4] = adcData[1] & 0xFF;
	txFrame[5] = button_state;
	txFrame[6] = keypad_val;
	txFrame[7] = txFrame[0]^txFrame[1]^txFrame[2]^txFrame[3]
	           ^ txFrame[4]^txFrame[5]^txFrame[6];

	DMA_TX_Send((uint8_t*)txFrame, 8);
}
//void Send_Char(uint8_t data)
//{
//	while(!(USART2->SR & USART_SR_TXE));
//	USART2->DR = data;
//}
//
//void Send_String(char *data)
//{
//	while(*data) Send_Char(*data++);
//}
//
//void Enable_Interrupt(void)
//{
//	USART2->CR1 |= USART_CR1_RXNEIE;
//	NVIC_EnableIRQ(USART2_IRQn);
//}
//
//void USART2_IRQHandler(void)
//{
//	if((USART2->SR & USART_SR_RXNE))
//	{
//		rx_char = USART2->DR;
//		rx_ready = 1;
//	}
//}
