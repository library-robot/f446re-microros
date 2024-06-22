/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "uart_receive_dma.h"
#include "usart.h"

#include "scan_motor.h"

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <uxr/client/transport.h>
#include <rmw_microxrcedds_c/config.h>
#include <rmw_microros/rmw_microros.h>

#include <std_msgs/msg/string.h>
#include <sensor_msgs/msg/imu.h>

#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
rcl_publisher_t publisher_string_scan;
rcl_publisher_t publisher_string_pos;
std_msgs__msg__String pub_str_msg;
std_msgs__msg__String pub_str_pos;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart6;
extern queue8_t uart_queue;

extern uint8_t BLE_RX_BUF[100];
extern TIM_HandleTypeDef htim1;
#define transmitSignal 0x0001
#define transmitSignal2 0x0002
#define positionSignal 0x0003



int motor_status = 0;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId rfidExecuteTaskHandle;
osThreadId tagNumTransmitTHandle;
osThreadId motorTaskHandle;
osThreadId transmitTask2Handle;
osThreadId RosTaskHandle;
uint32_t RosTaskBuffer[ 10000 ];
osStaticThreadDef_t RosTaskControlBlock;
osThreadId transePositionHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartRfidExecuteTask(void const * argument);
void StartTagNumTransmitTask(void const * argument);
void controlmotor(void const * argument);
void StarttransmitTask2(void const * argument);
void StartRosTask(void const * argument);
void StartTransePosition(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityHigh, 0, 400);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of rfidExecuteTask */
  osThreadDef(rfidExecuteTask, StartRfidExecuteTask, osPriorityNormal, 0, 128);
  rfidExecuteTaskHandle = osThreadCreate(osThread(rfidExecuteTask), NULL);

  /* definition and creation of tagNumTransmitT */
  osThreadDef(tagNumTransmitT, StartTagNumTransmitTask, osPriorityNormal, 0, 128);
  tagNumTransmitTHandle = osThreadCreate(osThread(tagNumTransmitT), NULL);

  /* definition and creation of motorTask */
  osThreadDef(motorTask, controlmotor, osPriorityLow, 0, 128);
  motorTaskHandle = osThreadCreate(osThread(motorTask), NULL);

  /* definition and creation of transmitTask2 */
  osThreadDef(transmitTask2, StarttransmitTask2, osPriorityNormal, 0, 128);
  transmitTask2Handle = osThreadCreate(osThread(transmitTask2), NULL);

  /* definition and creation of RosTask */
  osThreadStaticDef(RosTask, StartRosTask, osPriorityLow, 0, 10000, RosTaskBuffer, &RosTaskControlBlock);
  RosTaskHandle = osThreadCreate(osThread(RosTask), NULL);

  /* definition and creation of transePosition */
  osThreadDef(transePosition, StartTransePosition, osPriorityIdle, 0, 128);
  transePositionHandle = osThreadCreate(osThread(transePosition), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
	read_rfid_number();
	read_location();
//	read_command();

    osDelay(50);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartRfidExecuteTask */
/**
* @brief Function implementing the rfidExecuteTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartRfidExecuteTask */
void StartRfidExecuteTask(void const * argument)
{
  /* USER CODE BEGIN StartRfidExecuteTask */
	  uint8_t read_tag_single_time[] = {0xBB, 0x00, 0x22, 0x00, 0x00, 0x22, 0x7E};
	  vTaskSuspend(rfidExecuteTaskHandle);
	  /* Infinite loop */
	  for(;;)
	  {
		HAL_UART_Transmit(&huart1, read_tag_single_time, sizeof(read_tag_single_time), HAL_MAX_DELAY);

		//vTaskResume(defaultTaskHandle);
	    osDelay(300);
	  }
  /* USER CODE END StartRfidExecuteTask */
}

/* USER CODE BEGIN Header_StartTagNumTransmitTask */
/**
* @brief Function implementing the tagNumTransmitT thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTagNumTransmitTask */
void StartTagNumTransmitTask(void const * argument)
{
  /* USER CODE BEGIN StartTagNumTransmitTask */
  /* Infinite loop */
  osEvent event;

  for(;;)
  {
//	  HAL_UART_Receive_IT(&huart5, BLE_RX_BUF,12);
	event = osSignalWait(transmitSignal, 100);
	if(event.value.signals == transmitSignal){
		transmitData();
		}
	osDelay(10);
  }


  /* USER CODE END StartTagNumTransmitTask */
}

/* USER CODE BEGIN Header_controlmotor */
/**
* @brief Function implementing the motorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_controlmotor */
void controlmotor(void const * argument)
{
  /* USER CODE BEGIN controlmotor */
  /* Infinite loop */
	vTaskSuspend(motorTaskHandle);
  /* Infinite loop */
  for(;;)
  {
	  if(motor_status == 0){
		  scan_motor_up();
	  }else if(motor_status == 1){
		  scan_motor_down();
	  }
	  osDelay(100);
  }
  /* USER CODE END controlmotor */
}

/* USER CODE BEGIN Header_StarttransmitTask2 */
/**
* @brief Function implementing the transmitTask2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StarttransmitTask2 */
void StarttransmitTask2(void const * argument)
{
  /* USER CODE BEGIN StarttransmitTask2 */
	osEvent event;
  /* Infinite loop */
	  for(;;)
	  {
		  HAL_UART_Receive_IT(&huart5, BLE_RX_BUF,12);
		  event = osSignalWait(transmitSignal2, 100);
		  if(event.value.signals == transmitSignal2){
			  transmitCommand();
		  }
		  osDelay(1);
	  }
  /* USER CODE END StarttransmitTask2 */
}

/* USER CODE BEGIN Header_StartRosTask */
void * microros_allocate(size_t size, void * state);
void microros_deallocate(void * pointer, void * state);
void * microros_reallocate(void * pointer, size_t size, void * state);
void * microros_zero_allocate(size_t number_of_elements, size_t size_of_element, void * state);
bool cubemx_transport_open(struct uxrCustomTransport * transport);
bool cubemx_transport_close(struct uxrCustomTransport * transport);
size_t cubemx_transport_write(struct uxrCustomTransport* transport, const uint8_t * buf, size_t len, uint8_t * err);
size_t cubemx_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err);
#define ARRAY_LEN 200
#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc); return 1;}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}

void subscription_str_callback(const void * msgin)
{
  std_msgs__msg__String * msg = (std_msgs__msg__String *)msgin;
  pub_str_msg = *msg;
  char str[100];
  strcpy(str, msg->data.data);
  if(!strcmp(str,"scan")){
	  sprintf(pub_str_msg.data.data, "launch", str);
	  vTaskResume(motorTaskHandle);
	  vTaskResume(rfidExecuteTaskHandle);
  }else{
	  sprintf(pub_str_msg.data.data, "F446RE heard: %s", str);
  }
  //sprintf(pub_str_msg.data.data, "F446RE heard: %s", str);
  pub_str_msg.data.size = strlen(pub_str_msg.data.data);
  //rcl_publish(&publisher_string_scan, &pub_str_msg, NULL);
  //Publisher_test();
  //debug_led();
}

void debug_led()
{
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); //LED turned on
  HAL_Delay(200); //Wait for 200[ms]
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); //LED turned off
  HAL_Delay(200);
}

/**
* @brief Function implementing the RosTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartRosTask */
void StartRosTask(void const * argument)
{
  /* USER CODE BEGIN StartRosTask */
	  // micro-ROS configuration
	  char test_array[ARRAY_LEN];
	  memset(test_array,'z',ARRAY_LEN);

	  rmw_uros_set_custom_transport(
	    true,
	    (void *) &huart6,
	    cubemx_transport_open,
	    cubemx_transport_close,
	    cubemx_transport_write,
	    cubemx_transport_read);

	  rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();
	  freeRTOS_allocator.allocate = microros_allocate;
	  freeRTOS_allocator.deallocate = microros_deallocate;
	  freeRTOS_allocator.reallocate = microros_reallocate;
	  freeRTOS_allocator.zero_allocate =  microros_zero_allocate;

	  if (!rcutils_set_default_allocator(&freeRTOS_allocator)) {
	    printf("Error on default allocators (line %d)\n", __LINE__);
	  }

	  rcl_subscription_t subscriber_oper;
	  std_msgs__msg__String sub_str_msg;
	  sensor_msgs__msg__Imu sub_imu_msg;
	  rclc_support_t support;
	  rcl_allocator_t allocator;
	  rcl_node_t node;

	  allocator = rcl_get_default_allocator();

	  // create init_options
	  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

	  // create node
	  RCCHECK(rclc_node_init_default(&node, "f446re_node", "", &support));

	  // create publisher
	  RCCHECK(rclc_publisher_init_best_effort(
	    &publisher_string_scan,
	    &node,
	    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
	    "/f446re_scan_status"));

	  RCCHECK(rclc_publisher_init_best_effort(
	    &publisher_string_pos,
	    &node,
	    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
	    "/f446re_nav_pos"));

	  // create subscriber
	  RCCHECK(rclc_subscription_init_default(
	    &subscriber_oper,
	    &node,
	    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
	    "/f446re_scan_oper"));

	  // create executor
	  rclc_executor_t executor = rclc_executor_get_zero_initialized_executor();
	  RCCHECK(rclc_executor_init(&executor, &support.context, 2, &allocator));
	  RCCHECK(rclc_executor_add_subscription(&executor, &subscriber_oper, &sub_str_msg, &subscription_str_callback, ON_NEW_DATA));

	  // initialize message memory
	  pub_str_msg.data.data = (char * ) malloc(ARRAY_LEN * sizeof(char));
	  pub_str_msg.data.size = 0;
	  pub_str_msg.data.capacity = ARRAY_LEN;

	  pub_str_pos.data.data = (char * ) malloc(ARRAY_LEN * sizeof(char));
	  pub_str_pos.data.size = 0;
	  pub_str_pos.data.capacity = ARRAY_LEN;

	  sub_str_msg.data.data = (char * ) malloc(ARRAY_LEN * sizeof(char));
	  sub_str_msg.data.size = 0;
	  sub_str_msg.data.capacity = ARRAY_LEN;

	  sub_imu_msg.header.frame_id.capacity = 100;
	  sub_imu_msg.header.frame_id.data =(char * ) malloc(100 * sizeof(char));
	  sub_imu_msg.header.frame_id.size = 0;

	  // execute subscriber
	  rclc_executor_spin(&executor);
	  // cleaning Up
	  RCCHECK(rcl_publisher_fini(&publisher_string_scan, &node));
	  RCCHECK(rcl_publisher_fini(&publisher_string_pos, &node));
	  RCCHECK(rcl_subscription_fini(&subscriber_oper, &node));
	  RCCHECK(rcl_node_fini(&node));
	  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartRosTask */
}

/* USER CODE BEGIN Header_StartTransePosition */
/**
* @brief Function implementing the transePosition thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTransePosition */
void StartTransePosition(void const * argument)
{
  /* USER CODE BEGIN StartTransePosition */
  /* Infinite loop */
	osEvent event;
  for(;;)
  {
	  event = osSignalWait(positionSignal, 100);
	  	if(event.value.signals == positionSignal){
	  		char xChar[6],yChar[6];
			HAL_UART_Transmit(&huart2, BLE_RX_BUF, 12, 1000);
			if(BLE_RX_BUF[0] == 'z'){
				//정비시작
				int a;
			}
			else{
				//book location move
				for(int i=0;i<6;i++){
					xChar[i] = BLE_RX_BUF[i];
					yChar[i] = BLE_RX_BUF[i+6];
				}
				Publisher_pos(xChar,yChar);
			}

			HAL_UART_Receive_IT(&huart5, BLE_RX_BUF,12);

			memset(BLE_RX_BUF,0,sizeof(BLE_RX_BUF));
	  	}

    osDelay(1);
  }
  /* USER CODE END StartTransePosition */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
