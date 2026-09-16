/*
 * adc.h
 *
 *  Created on: Jul 21, 2026
 *      Author: anton
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

void ADC_Init(void);
void ADC_Enable(void);
void ADC_Start (int channel);
void ADC_WaitForConv (void);
uint16_t ADC_GetVal (void);
void ADC_Disable (void);

#endif /* INC_ADC_H_ */
