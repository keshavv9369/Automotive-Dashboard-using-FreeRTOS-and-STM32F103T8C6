#ifndef UI_H_
#define UI_H_

#include "stm32f103xb.h"

/* Main Menu UI Functions */
void print_mainmenu_speed(void);
void print_mainmenu_rpm(void);
void print_mainmenu_temp(void);
void print_mainmenu_throttle(void);

/* Parameter Display UI Functions */
void print_parameter_speed(void);
void print_parameter_rpm(void);
void print_parameter_temp(void);
void print_parameter_throttle(void);

/* Utility Function */
void Int_To_String(uint16_t value, char *buffer);

#endif /* UI_H_ */
