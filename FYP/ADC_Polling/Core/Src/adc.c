/*
 * adc.c
 *
 *  Created on: Jul 20, 2026
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
	 */



	/* 1.Enable ADC and GPIO clock */
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;		// enable ADC! clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	// enable GPIOA clock

	/* 2. Set the prescalar in the Custom Control Register (CCR) */
	ADC->CCR |= (ADC_CCR_ADCPRE_0);			// 01: PCLK2 divided by 4

	/* 3. Set the Scan Mode and Resolution in the Control Register 1 (CR1) */
	ADC1->CR1 |= ADC_CR1_SCAN;				// SCAN mode enabled
	ADC1->CR1 &= ~(ADC_CR1_RES);			// 12 bit RESOLUTION

	/* 4. Set the Continuous, EOC and Data Alignment in Control Reg 2(CR2) */
	ADC1->CR2 |= ADC_CR2_CONT;				// enable continuous conversion
	ADC1->CR2 |= ADC_CR2_EOCS;				// EOC after each conversion
	ADC1->CR2 &= ~(ADC_CR2_ALIGN);			// Data Alignment RIGHT

	/* 5. Set the Sampling Time for the channels in ADC SMAPx */
	ADC1->SMPR2 |= (ADC_SMPR2_SMP0_2 | ADC_SMPR2_SMP1_2);	// Sampling time of 84 cycles for channel 1 and 2

	/* 6. Set the Regular channel sequence length in ADC_SQR1 */
	ADC1->SQR1 |= ADC_SQR1_L_0;					// SQR1_L =1 for 2c conversion

	/* 7. Set the Respective GPIO PINs in the Analog Mode */
	GPIOA->MODER |= GPIO_MODER_MODE0;			// analog mode for PA0
	GPIOA->MODER |= GPIO_MODER_MODE1;			// analog mode for PA1

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

void ADC_Start (int channel)
{
	/************** STEPS TO FOLLOW *****************
	1. Set the channel Sequence in the SQR Register
	2. Clear the Status register
	3. Start the Conversion by Setting the SWSTART bit in CR2
	************************************************/


/**	Since we will be polling for each channel, here we will keep one channel in the sequence at a time
		ADC1->SQR3 |= (channel<<0); will just keep the respective channel in the sequence for the conversion **/

	ADC1->SQR3 = 0;
	ADC1->SQR3 |= (channel<<0);    // conversion in regular sequence

	ADC1->SR = 0;        // clear the status register

	ADC1->CR2 |= ADC_CR2_SWSTART;  // start the conversion
}


void ADC_WaitForConv (void)
{
	/*************************************************
	EOC Flag will be set, once the conversion is finished
	*************************************************/
	while (!(ADC1->SR & (ADC_SR_EOC)));  // wait for EOC flag to set
}

uint16_t ADC_GetVal (void)
{
	return ADC1->DR;  // Read the Data Register
}

void ADC_Disable (void)
{
	/************** STEPS TO FOLLOW *****************
	1. Disable the ADC by Clearing ADON bit in CR2
	************************************************/
	ADC1->CR2 &= ~(ADC_CR2_ADON);  // Disable ADC
}









