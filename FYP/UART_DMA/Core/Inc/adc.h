/*
 * adc.h
 *
 *  Created on: Jul 23, 2026
 *      Author: anton
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

extern volatile uint16_t adcData[3];

void ADC_DMA_Init(void);
void ADC_Init(void);
void ADC_Enable(void);
void ADC_Start (void);
void ADC_Disable (void);
#endif /* INC_ADC_H_ */
