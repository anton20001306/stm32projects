/*
 * adc.c
 *
 *  Created on: Jul 23, 2026
 *      Author: anton
 */
#include "stm32f446xx.h"
#include "stdio.h"

volatile uint16_t adcData[3];   // [0]=PA1 ch1, [1]=PA4 ch4, [2]=temp ch18

void ADC_Init(void)
{

	/*********************************** STEPS TO FOLLOW ***********************************
	 * 1. Enable ADC and GPIO clock
	 * 2. Set the prescalar in the Custom Control Register (CCR)
	 * 3. Set the Scan Mode and Resolution in the Control Register 1 (CR1)
	 * 4. Set the Continuous, EOC and Data Alignment in Control Reg 2(CR2)
	 * 5. Set the Sampling Time for the channels in ADC SMAPx
	 * 6. Set the Regular channel sequence length in ADC_SQR1
	 * 7. Set the Respective GPIO PINs in the Analog Mode
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
	ADC1->SMPR2 &= ~(ADC_SMPR2_SMP1 | ADC_SMPR2_SMP4);      // clear both fields
	ADC1->SMPR2 |=  (ADC_SMPR2_SMP1_2 | ADC_SMPR2_SMP4_2);  // 0b100 = 84 cycles

	/* 6. Set the Regular channel sequence length in ADC_SQR1 */
	ADC1->SQR1 &= ~ADC_SQR1_L;      // clear the field first
	ADC1->SQR1 |= ADC_SQR1_L_1;     // L = 0b10 = 2 → 3 conversions

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

	/* Keep DMA requests coming in continuous mode */
	ADC1->CR2 |= ADC_CR2_DDS;

	/* Enable Continuous Request */
	ADC1->CR2 |= ADC_CR2_CONT;

	/* Channel Sequence 1st, 2nd and 3rd conversion */
	ADC1->SQR3 = (1 << 0) | (4 << 5) | (18 << 10);   // SQ1=ch1, SQ2=ch4, SQ3=ch18(temp)

}



void ADC_DMA_Init(void)
{
	/*Enable clock access to DMA2*/
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;		// ADC1 maps to DMA2, not DMA1

	/*Stream must be off before configuring*/
	DMA2_Stream0->CR &= ~(1 << 0);
	while(DMA2_Stream0->CR & (1 << 0));		// wait until hardware confirms off

	/*Select channel 0 for the stream*/
	DMA2_Stream0->CR &= ~(0x7 << 25);		// CHSEL = 000 : ADC1

	/*Data direction*/
	DMA2_Stream0->CR &= ~(0x3 << 6);		// DIR = 00 : peripheral-to-memory

	/*Circular mode*/
	DMA2_Stream0->CR |= (0x1 << 8);			// CIRC = 1

	/*Memory increment*/
	DMA2_Stream0->CR |= (0x1 << 10);		// MINC = 1

	/*Peripheral data size*/
	DMA2_Stream0->CR &= ~(0x3 << 11);
	DMA2_Stream0->CR |=  (0x1 << 11);		// PSIZE = 01 : 16 bit

	/*Memory data size*/
	DMA2_Stream0->CR &= ~(0x3 << 13);
	DMA2_Stream0->CR |=  (0x1 << 13);		// MSIZE = 01 : 16 bit

	/*Priority level*/
	DMA2_Stream0->CR &= ~(0x3 << 16);		// PL = 00 : low

	/*Peripheral address*/
	DMA2_Stream0->PAR  = (uint32_t)&ADC1->DR;

	/*Memory address*/
	DMA2_Stream0->M0AR = (uint32_t)adcData;

	/*Number of transfers*/
	DMA2_Stream0->NDTR = 3;					// 3 channels per sweep

	/*Enable the stream - last step*/
	DMA2_Stream0->CR |= (1 << 0);			// EN = 1
}


void ADC_Enable(void)
{
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

void ADC_Disable (void)
{
	/************** STEPS TO FOLLOW *****************
	1. Disable the ADC by Clearing ADON bit in CR2
	************************************************/
	ADC1->CR2 &= ~(ADC_CR2_ADON);  // Disable ADC
}
