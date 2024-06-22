/*
 * uart_receive_dma.h
 *
 *  Created on: Feb 8, 2024
 *      Author: Yoo
 */

#ifndef INC_UART_RECEIVE_DMA_H_
#define INC_UART_RECEIVE_DMA_H_

#define QUEUE_BUF_MAX   256

typedef struct queue{
  uint8_t  buf[QUEUE_BUF_MAX];
  uint16_t q_in_index;
  uint16_t q_out_index;
} queue8_t;

uint32_t uart_available(void);
uint8_t uart_q8_read(void);
void uart_init(void);
void read_rfid_number(void);
void transmitData();

uint32_t uart5_available(void);
uint8_t uart5_q8_read(void);
void read_rfid_number5(void);
void transmitData5();

uint32_t uart2_available(void);
uint8_t uart2_q8_read(void);
void uart2_init(void);
void read_command(void);
void transmitCommand();
void read_location();

#endif /* INC_UART_RECEIVE_DMA_H_ */
