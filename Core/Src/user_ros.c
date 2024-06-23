/*
 * user_ros.c
 *
 *  Created on: May 16, 2024
 *      Author: hwabok
 */

#include <std_msgs/msg/string.h>
#include <rcl/rcl.h>
#include "usart.h"

extern rcl_publisher_t publisher_string_scan;
extern rcl_publisher_t publisher_string_pos;
extern UART_HandleTypeDef huart2;
extern char xChar[6],yChar[6];

void Publisher_state(){
	std_msgs__msg__String msg;
	char str[20] = "finish";
	msg.data.data = str;
	msg.data.size = strlen(str);
	rcl_publish(&publisher_string_scan, &msg, NULL);
}

void Publisher_test(){
	std_msgs__msg__String msg;
	char str[20] = "test";
	msg.data.data = str;
	msg.data.size = strlen(str);
	rcl_publish(&publisher_string_scan, &msg, NULL);
}

/*double type input X,Y */
void Publisher_pos(){
	std_msgs__msg__String msg;
	char str[100] = "";
	sprintf(str, "x%s y%s", xChar,yChar);
	msg.data.data = str;
	msg.data.size = strlen(str);
	//HAL_UART_Transmit(&huart2, str, 12, 1000);
	rcl_publish(&publisher_string_pos, &msg, NULL);
}
