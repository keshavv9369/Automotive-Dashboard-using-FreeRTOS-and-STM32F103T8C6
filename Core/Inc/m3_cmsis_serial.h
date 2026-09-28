#ifndef M3_CMSIS_SERIAL_H
#define M3_CMSIS_SERIAL_H

#include "stm32f103xb.h"

void serial_usart1_Init(void);
void serial_usart1_char_print(char *str);
void serial_usart1_var_print(int value);

#endif