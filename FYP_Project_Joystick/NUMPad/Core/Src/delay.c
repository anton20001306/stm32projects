#include "stm32f446xx.h"


void Tim2Init()
{
	/*Enable clock access to timer2*/
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->PSC = 84-1;

	TIM2->EGR = TIM_EGR_UG;
	TIM2->SR &= ~TIM_SR_UIF;

}

void DelayInit(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	GPIOA->MODER &= ~GPIO_MODER_MODE5;
	GPIOA->MODER |=  GPIO_MODER_MODE5_0;

	GPIOA->OTYPER &= ~GPIO_OTYPER_OT5;

	GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR5;

	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5;

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

/************************************* 50Hz ******************************* */
void Tim6Init(void)
{
	/*Enable clock access to timer6*/
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

	/*84 MHz / 8400 = 10 kHz tick (100 us per count)*/
	TIM6->PSC = 8400-1;

	/*10 kHz / 200 = 50 Hz (20 ms period)*/
	TIM6->ARR = 200-1;

	/*Load PSC/ARR from shadow registers*/
	TIM6->EGR = TIM_EGR_UG;
	TIM6->SR &= ~TIM_SR_UIF;

	/*Enable update interrupt*/
	TIM6->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM6_DAC_IRQn);

	/*Start free-running*/
	TIM6->CR1 |= TIM_CR1_CEN;
}

void TIM6_DAC_IRQHandler(void)
{
	if(TIM6->SR & TIM_SR_UIF)
	{
		TIM6->SR &= ~TIM_SR_UIF;
		//Tim6_Tick();
	}
}