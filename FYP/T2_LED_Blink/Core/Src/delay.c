#include "stm32f446xx.h"

void Tim2Init()
{
	/*Enable clock access to timer2*/
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->PSC = 16-1;

	TIM2->EGR = TIM_EGR_UG;
	TIM2->SR &= ~TIM_SR_UIF;

}

void delay_us(uint16_t us)
{
	TIM2->ARR = us-1;
	TIM2->CNT = 0;
	TIM2->SR  &= ~TIM_SR_UIF;
	TIM2->CR1 |= TIM_CR1_CEN;

	while(!(TIM2->SR & TIM_SR_UIF)){};

	TIM2->SR &= ~TIM_SR_UIF;
	TIM2->CR1 &= ~TIM_CR1_CEN;
}

void delay_ms(uint16_t ms)
{
	while(ms--){delay_us(1000);}
}
