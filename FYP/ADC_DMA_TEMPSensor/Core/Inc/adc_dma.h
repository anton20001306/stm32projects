/*
 * adc_dma.h
 *
 *  Created on: Jul 21, 2026
 *      Author: anton
 */

#ifndef INC_ADC_DMA_H_
#define INC_ADC_DMA_H_

void ADC_Init(void);
void ADC_Enable(void);
void ADC_Start ();
void DMA_Init(void);
void DMA_Config (uint32_t srcAdd, uint32_t destAdd, uint16_t size);
void ADC_Disable (void);
#endif /* INC_ADC_DMA_H_ */
