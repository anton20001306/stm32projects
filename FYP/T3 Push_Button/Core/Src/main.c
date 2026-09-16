#include "stm32f446xx.h"
#include "main.h"

void SystemClock_Config(void);

volatile uint8_t buttonPressed = 0;
volatile uint32_t buttonPressedConfidenceLevel = 0;
volatile uint32_t buttonReleasedConfidenceLevel = 0;
volatile uint32_t confidenceThreshold = 200;

volatile uint8_t LEDState = 0;

void GpioInit()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	GPIOA->MODER &= ~GPIO_MODER_MODE5;
	GPIOA->MODER |=  GPIO_MODER_MODE5_0;

	GPIOA->OTYPER &= ~GPIO_OTYPER_OT5;

	GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR5;

	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5;


	GPIOC->MODER &= ~GPIO_MODER_MODE13;

	GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13;
	GPIOC->PUPDR |= GPIO_PUPDR_PUPD13_0;


}

int main(void)
{

  HAL_Init();
  SystemClock_Config();

  GpioInit();

   while (1)
  {
	   if(!(GPIOC->IDR & GPIO_IDR_ID13))
	   {
		   //GPIOA->BSRR |= GPIO_BSRR_BS5;
		   if(buttonPressed == 0)
		   {
			   if(buttonPressedConfidenceLevel > confidenceThreshold)
			   {
				   if(LEDState == 0)
				   {
					   LEDState = 1;
					   GPIOA->BSRR |= GPIO_BSRR_BS5;
				   }
				   else
				   {
					   LEDState = 0;
					   GPIOA->BSRR |= GPIO_BSRR_BR5;

				   }

				   buttonPressed = 1;
			   }
			   else
			   {
				   buttonPressedConfidenceLevel++;
				   buttonReleasedConfidenceLevel = 0;
			   }
		   }
	   }
	   else
	   {
		   //GPIOA->BSRR |= GPIO_BSRR_BR5;
		   if(buttonPressed == 1)
		   {
			   if(buttonReleasedConfidenceLevel > confidenceThreshold)
			   {
				   buttonPressed = 0;
			   }
			   else
			   {
				   buttonReleasedConfidenceLevel++;
				   buttonPressedConfidenceLevel = 0;
			   }
		   }
	   }
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
