#include "stm32f103xb.h"


void serial_usart1_Init(void){
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	GPIOA->CRH &= ~(0xF << 4);
	GPIOA->CRH |=  (0xB << 4);

	GPIOA->CRH &= ~(0xF << 8);
	GPIOA->CRH |=  (0x4 << 8);

	USART1->CR1 =0;
	USART1->CR2 =0;
	USART1->CR3 =0;

	USART1->CR1 |=  USART_CR1_TE |
					USART_CR1_RE|
					USART_CR1_UE;
}
void serial_usart1_char_print(char *str)
	{
	    while(*str)
	    {
	        while(!(USART1->SR & USART_SR_TXE));
	        USART1->DR = *str;
	        str++;
	    }
	}

void serial_usart1_var_print(int value)
{
    char buffer[12];
    int i = 0;

    if(value == 0)
    {
        while(!(USART1->SR & USART_SR_TXE));
        USART1->DR = '0';
        return;
    }

    if(value < 0)
    {
        while(!(USART1->SR & USART_SR_TXE));
        USART1->DR = '-';
        value = -value;
    }

    while(value > 0)
    {
        buffer[i] = (value % 10) + '0';
        value = value / 10;
        i++;
    }

    while(i > 0)
    {
        i--;
        while(!(USART1->SR & USART_SR_TXE));
        USART1->DR = buffer[i];
    }
}
