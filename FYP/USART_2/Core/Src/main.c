#include "stm32f446xx.h"
#include "main.h"

void SystemClock_Config(void);

void Gpio_Init()
{
	/*RCC clock enable*/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;	// GPIO A enable
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;	// USART2  enable

	/*Configure USART pins PA2 and PA3*/
	GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
	GPIOA->MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1);

	/*Set Alternate function mode*/
	GPIOA->AFR[0] &= ~(0xFFF << (4 * 2));
	GPIOA->AFR[0] |=  (0x777 << (4 * 2));

	/*Set high speed*/
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDER_OSPEEDR3);
	GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR2 | GPIO_OSPEEDER_OSPEEDR3);

	/*USART2 initialization*/
	USART2->CR1 = 0;	// Reset the CR1
	USART2->CR1 |= USART_CR1_UE;
	USART2->CR1 |= USART_CR1_TE | USART_CR1_RE;

	/*Define 8 bit data length*/
	USART2->CR1 &= ~(USART_CR1_M);

	/*Define the baud rate*/
	USART2->BRR = (8 << 4) | (11);	// FCLK = 16MHz and BaudRate = 115200


}
void Send_Char(uint8_t data)
{
	while(!(USART2->SR & (1 << 6)));
	USART2->DR = data;
}

void Send_String(char *data)
{
	while(*data) Send_Char(*data++);
}

uint8_t Read_Char()
{
	uint8_t temp;
	while(!(USART2->SR & USART_SR_RXNE));
	temp = USART2->DR;
	return temp;
}
int main(void)
{

  HAL_Init();

  SystemClock_Config();
  Gpio_Init();

  while (1)
  {
//	  Transmit('c');
//	  Send_String("Hello world! \n");
	  uint8_t dataReceive = Read_Char();
	  Send_Char(dataReceive);

  }

}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
