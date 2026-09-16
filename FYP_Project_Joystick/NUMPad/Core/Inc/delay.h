/*
 * delay.h
 *
 *  Created on: Jul 10, 2026
 *      Author: anton
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

#include <stdint.h>

void DelayInit(void);
void Tim2Init();
void delay_us(uint16_t us);
void delay_ms(uint16_t ms);

void Tim6Init(void);
void TIM6_DAC_IRQHandler(void);

#endif /* INC_DELAY_H_ */
