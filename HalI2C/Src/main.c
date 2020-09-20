/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include "raine.c"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define OLED_ADDR 0x3c << 1
#define CMD_ADDR 0x00
#define DATA_ADDR 0x40

#define PIX_PER_COL 8
#define PAGES_PER_SCREEN 4
#define COLS_PER_PAGE 128
#define WIDTH COLS_PER_PAGE
#define HEIGHT PIX_PER_COL * PAGES_PER_SCREEN  
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void write_cmd_oled(uint8_t cmd)
{
    //Send command one byte at at time
    HAL_I2C_Mem_Write(&hi2c1,OLED_ADDR, CMD_ADDR, 1, &cmd, 1, 100);
}

void write_data_oled(uint8_t* dataBuffer, size_t dataSize)
{
    //Send command one byte at at time
    HAL_I2C_Mem_Write(&hi2c1,OLED_ADDR, DATA_ADDR, 1, dataBuffer, dataSize, 100);
}
/* USER CODE END 0 */

void fill_black(uint8_t* buffer, size_t sz)
{
  // fill page buffer with black
  //uint8_t blck[128] = {0}
  for (int i = 0 ; i < sz ; i++)
  {
    buffer[i]=0; 
  }
//  HAL_I2C_Mem_Write(&hi2c1,OLED_ADDR, DATA_ADDR, 1, blck, 128, 100);
}

void draw_line_page(int h, uint8_t *buffer, size_t sz)
{
  //Draws horizontal line in screen at heigth h on a single page buffer
  int line = 1 << h;
  for (int i = 0 ; i < sz; i++)
  {
    buffer[i] = line;
  }
}

void draw_line(int h, uint8_t *buffer, size_t sz)
{
  int page = h / PIX_PER_COL;
  int offset = h - page*PIX_PER_COL;
  int value = 1 << offset;
  for (int i=0 ; i< sz; i++){
    if ( i / COLS_PER_PAGE == page  )
    {
       buffer[i] = value; 
    }
    else 
    {
      buffer[i] = 0;
    }
  }

  return;
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  uint8_t turn_on = 0xAF;
  uint8_t turn_off = 0xAE;
  write_cmd_oled(turn_off);
  //Set Memory addressing type
  write_cmd_oled(0x20);
  write_cmd_oled(0x00);
  //Set memory page start 
  write_cmd_oled(0xB0);
  //Mirroring
  write_cmd_oled(0xC8);
  write_cmd_oled(0xa1);
  
  //set columna low and high addr
  write_cmd_oled(0x00);
  write_cmd_oled(0x10);
  //Display start line : 0 -63 40 for 0
  write_cmd_oled(0x40);
  //Contrast 
  write_cmd_oled(0x81);
  write_cmd_oled(0xFF);
  //Set mux
  write_cmd_oled(0xa8);
  write_cmd_oled(0x1F);
  //Follow RAM
  write_cmd_oled(0xA4);
  //No offset 
  write_cmd_oled(0xD3);
  write_cmd_oled(0x00);

  //timing
  //OScilattor freq
  write_cmd_oled(0xD5);
  write_cmd_oled(0xF0);
  //Pre-charge
  write_cmd_oled(0xD9);
  write_cmd_oled(0x22);
  //Com pins
  write_cmd_oled(0xDA);
  write_cmd_oled(0x02);

  //Turn on
  //Vcomh
  write_cmd_oled(0xDB);
  write_cmd_oled(0x20);
  //Charge pump
  write_cmd_oled(0x8D);
  write_cmd_oled(0x14);
  write_cmd_oled(turn_on);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int i = 0;
  int j = 0;
  size_t buff_len = COLS_PER_PAGE*PAGES_PER_SCREEN; //128*4
  uint8_t buffer[buff_len];
  for(int j=0;j < buff_len; j++)
  {
   buffer[j]=0x00;
  }


  //Image?
  size_t line_len = 32;
//Original
 /* uint8_t first_line[] =  {  255,255,249,249,249,225, 39, 39, 30,134,134,  
                             1,  1,  1,  1,  1,  1,  1,  1,  1,  1,134,134,
                             30, 39, 39,225,249,249,249,255,255};
  uint8_t second_line[] = {  1,   1,255,131,131,241, 12, 12,130,113,113,
                             236, 12, 12, 16, 96, 96, 16, 12, 12,236,113,
                             113, 130, 12, 12,241,131,131,255,  1,  1}; 
  uint8_t third_line[] =  {  0,  0, 31,225,225, 29, 98, 98,131,  0,  0,  
                             3,  0,  0, 96, 96, 96, 96,  0,  0,  3,  0, 
                             0,131, 98, 98, 29,225,225, 31,  0,  0};
  uint8_t fourth_line[] = {  0,  0,  0,  0,  0,  3,  7,  7,  0,  3,  3,  
                             4,  8,  8, 11, 11, 11, 11,  8,  8,  4,  3,  
                             3,  0,  7,  7,  3,  0,  0,  0,  0,  255 };
  
  memcpy(buffer,first_line,line_len);
  memcpy(buffer+128,second_line,line_len);
  memcpy(buffer+256,third_line,line_len);
  memcpy(buffer+384,fourth_line,line_len);
*/
  memcpy(buffer,raine,line_len);
  memcpy(buffer+128,raine+line_len,line_len);
  memcpy(buffer+256,raine+2*line_len,line_len);
  memcpy(buffer+384,raine+3*line_len,line_len);
  

  int state = 0;
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    //write_cmd_oled(0xB0+j);
    //No i have no idea why i have tro write twice
    //If i do not skips every pair line
    write_data_oled(buffer,buff_len);
    HAL_Delay(100);
    //draw_line(i,buffer, buff_len);
    //write_data_oled(buffer,buff_len);
    //HAL_Delay(100);
    //write_data_oled(buffer,buff_len);
   // write_data_oled(buffer,buff_len);
   // write_data_oled(buffer,buff_len);
   // write_data_oled(buffer,buff_len);
    switch (state)
    {
      case 0:
        i++;
        if (i > HEIGHT - 3){
           state = 1;
        }
        break;
      case 1:
        i--;
        if (i < 1 ){
            state = 0;
        }
        break;
    }
  
//    for( int j=0;j < buff_len; j++)

//    {
//      buffer[j]=buffer[j]>>1;
//    }
//    if (buffer[0] >= 127)
//    {
//      for( int j=0;j < buff_len; j++)
//      {
//        buffer[j]=1;
//      }
//    }
//
    
  }
  
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
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

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 5 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
