/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Limit_switch_up_Pin GPIO_PIN_13
#define Limit_switch_up_GPIO_Port GPIOC
#define com_TX_Pin GPIO_PIN_2
#define com_TX_GPIO_Port GPIOA
#define com_RX_Pin GPIO_PIN_3
#define com_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define esp32_RX_Pin GPIO_PIN_5
#define esp32_RX_GPIO_Port GPIOC
#define esp32_TX_Pin GPIO_PIN_10
#define esp32_TX_GPIO_Port GPIOB
#define Micro_ROS_TX_Pin GPIO_PIN_6
#define Micro_ROS_TX_GPIO_Port GPIOC
#define Micro_ROS_RX_Pin GPIO_PIN_7
#define Micro_ROS_RX_GPIO_Port GPIOC
#define Direction0_Pin GPIO_PIN_8
#define Direction0_GPIO_Port GPIOC
#define Direction1_Pin GPIO_PIN_9
#define Direction1_GPIO_Port GPIOC
#define UHF01_TX_Pin GPIO_PIN_9
#define UHF01_TX_GPIO_Port GPIOA
#define UHF01_RX_Pin GPIO_PIN_10
#define UHF01_RX_GPIO_Port GPIOA
#define F103re_TX_Pin GPIO_PIN_12
#define F103re_TX_GPIO_Port GPIOC
#define F103re_RX_Pin GPIO_PIN_2
#define F103re_RX_GPIO_Port GPIOD
#define Limit_switch_down_Pin GPIO_PIN_9
#define Limit_switch_down_GPIO_Port GPIOB
#define Limit_switch_down_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
