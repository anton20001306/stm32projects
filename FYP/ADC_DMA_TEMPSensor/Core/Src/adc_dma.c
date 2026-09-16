/*
 * adc_dma.c
 *
 *  Created on: Jul 21, 2026
 *      Author: anton
 */

#include "stm32f446xx.h"
#include "stdio.h"

void ADC_Init(void){

	/*********************************** STEPS TO FOLLOW ***********************************
	 * 1. Enable ADC and GPIO clock
	 * 2. Set the prescalar in the Custom Control Register (CCR)
	 * 3. Set the Scan Mode and Resolution in the Control Register 1 (CR1)
	 * 4. Set the Continuous, EOC and Data Alignment in Control Reg 2(CR2)
	 * 5. Set the Sampling Time for the channels in ADC SMAPx
	 * 6. Set the Regular channel sequence length in ADC_SQR1
	 * 7. 7. Set the Respective GPIO PINs in the Analog Mode
	 */



	/* 1.Enable ADC and GPIO clock */
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;		// enable ADC! clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	// enable GPIOA clock

	/* 2. Set the prescalar in the Custom Control Register (CCR) */
	ADC->CCR |= (ADC_CCR_ADCPRE_1);			// 01: PCLK2 divided by 6

	/* 3. Set the Scan Mode and Resolution in the Control Register 1 (CR1) */
	ADC1->CR1 |= ADC_CR1_SCAN;				// SCAN mode enabled
	ADC1->CR1 &= ~(ADC_CR1_RES);			// 12 bit RESOLUTION

	/* 4. Set the Continuous, EOC and Data Alignment in Control Reg 2(CR2) */
	ADC1->CR2 |= ADC_CR2_CONT;				// enable continuous conversion
	ADC1->CR2 |= ADC_CR2_EOCS;				// EOC after each conversion
	ADC1->CR2 &= ~(ADC_CR2_ALIGN);			// Data Alignment RIGHT

	/* 5. Set the Sampling Time for the channels in ADC SMAPx */
	ADC1->SMPR2 &= ~(ADC_SMPR2_SMP1 | ADC_SMPR2_SMP4);	// Sampling time of 3 cycles for channel 1 and 4
//	ADC1->SMPR1 |= ADC_SMPR1_SMP18;
	/* 6. Set the Regular channel sequence length in ADC_SQR1 */
	ADC1->SQR1 |= (ADC_SQR1_L_0 |ADC_SQR1_L_1);			// SQR1_L =2 for 3 conversion

	/* 7. Set the Respective GPIO PINs in the Analog Mode */
	GPIOA->MODER |= GPIO_MODER_MODE1;		// analog mode for PA1
	GPIOA->MODER |= GPIO_MODER_MODE4;		// analog mode for PA4

	/****************************************************************************************/

	/* Sampling freq for Temperature Sensor */
	ADC1->SMPR1 |= ADC_SMPR1_SMP18;

	/* Set the TSVREFE Bit */
	ADC->CCR |= ADC_CCR_TSVREFE;

	/* Enable DMA for ADC */
	ADC1->CR2 |= ADC_CR2_DMA;

	/* Enable Continuous Request */
	ADC1->CR2 |= ADC_CR2_CONT;

	/* Channel Sequence 1st, 2nd and 3rd conversion */
//	ADC1->SQR3 |= (ADC_SQR3_SQ1_0 | ADC_SQR3_SQ2_2 | ADC_SQR3_SQ3_4 | ADC_SQR3_SQ3_1);
	ADC1->SQR3 = (1 << 0) | (4 << 5) | (18 << 10);   // SQ1=ch1, SQ2=ch4, SQ3=ch18(temp)

}

void ADC_Enable(void){
	/*/*********************************** STEPS TO FOLLOW ***********************************
	 * 1. Enable the ADC by setting ADON bit in CR2
	 * 2. Wait forADC to stabilize (approx 10us)
	 */

	ADC1->CR2 |= ADC_CR2_ADON;   // ADON =1 enable ADC1

	uint32_t delay = 10000;
	while(delay--);
}

void ADC_Start (void)
{
	/************** STEPS TO FOLLOW *****************
	1. Clear the Status register
	2. Start the Conversion by Setting the SWSTART bit in CR2
	************************************************/

	ADC1->SR = 0;        // clear the status register

	ADC1->CR2 |= ADC_CR2_SWSTART;  // start the conversion
}

void DMA_Init(void)
{
	/************** STEPS TO FOLLOW *****************
	1. Enable DMA2 clock
	2. Set the DATA Direction
	3. Enable/Disable the Circular Mode
	4. Enable/Disable the Memory Increment and Peripheral Increment
	5. Set the Data Size
	6. Select the channel for the Stream
	************************************************/

	// Enable the DMA2 Clock
	RCC->AHB1ENR |= (RCC_AHB1ENR_DMA1EN);  	// DMA1EN = 1

	// Select the Data Direction
	DMA2_Stream0->CR &= ~(3<<6);  			// Peripheral to memory

	// Select Circular mode
	DMA2_Stream0->CR |= (1<<8);  			// CIRC = 1

	// Enable Memory Address Increment
	DMA2_Stream0->CR |= (1<<10);  			// MINC = 1;

	// Set the size for data
	DMA2_Stream0->CR |= (1<<11)|(1<<13);  	// PSIZE = 01, MSIZE = 01, 16 bit data

	// Select channel for the stream
	DMA2_Stream0->CR &= ~(7<<25);  	// Channel 0 selectedted
}

void DMA_Config (uint32_t srcAdd, uint32_t destAdd, uint16_t size)
{

	/************** STEPS TO FOLLOW *****************
	1. Set the Data Size in the CNDTR Register
	2. Set the Peripheral Address and the Memory Address
	3. Enable the DMA Stream

		 Some peripherals don't need a start condition, like UART, So as soon as you enable the DMA, the transfer will begin
		 While Peripherals like ADC needs the Start condition, so Start the ADC later in the program, to enable the transfer
	************************************************/

	DMA2_Stream0->NDTR = size;   // Set the size of the transfer

	DMA2_Stream0->PAR = srcAdd;  // Source address is peripheral address

	DMA2_Stream0->M0AR = destAdd;  // Destination Address is memory address

	// Enable the DMA Stream
	DMA2_Stream0->CR |= (DMA_SxCR_EN);  // EN =1
}



void ADC_Disable (void)
{
	/************** STEPS TO FOLLOW *****************
	1. Disable the ADC by Clearing ADON bit in CR2
	************************************************/
	ADC1->CR2 &= ~(ADC_CR2_ADON);  // Disable ADC
}

