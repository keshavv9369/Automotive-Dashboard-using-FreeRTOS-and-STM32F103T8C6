#include "gpio_task.h"
#include "stm32f103xb.h"

void Button_EXTI_Init(void)
{
    /* Enable GPIOB and AFIO clocks */
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

    /* PB12, PB13, PB14, PB15
       Input mode with Pull-Up / Pull-Down */

    GPIOB->CRH &= ~(
          GPIO_CRH_MODE12 | GPIO_CRH_CNF12
        | GPIO_CRH_MODE13 | GPIO_CRH_CNF13
        | GPIO_CRH_MODE14 | GPIO_CRH_CNF14
        | GPIO_CRH_MODE15 | GPIO_CRH_CNF15
    );

    GPIOB->CRH |= (
          GPIO_CRH_CNF12_1
        | GPIO_CRH_CNF13_1
        | GPIO_CRH_CNF14_1
        | GPIO_CRH_CNF15_1
    );

    /* Select internal Pull-Up */
    GPIOB->ODR |= (1U << 12) |
                  (1U << 13) |
                  (1U << 14) |
                  (1U << 15);

    /* Map EXTI12-15 to GPIOB */
    AFIO->EXTICR[3] &= ~(
          AFIO_EXTICR4_EXTI12
        | AFIO_EXTICR4_EXTI13
        | AFIO_EXTICR4_EXTI14
        | AFIO_EXTICR4_EXTI15
    );

    AFIO->EXTICR[3] |= (
          AFIO_EXTICR4_EXTI12_PB
        | AFIO_EXTICR4_EXTI13_PB
        | AFIO_EXTICR4_EXTI14_PB
        | AFIO_EXTICR4_EXTI15_PB
    );

    /* Unmask EXTI12-15 */
    EXTI->IMR |= (1U << 12) |
                 (1U << 13) |
                 (1U << 14) |
                 (1U << 15);

    /* Falling-edge trigger because buttons use Pull-Up */
    EXTI->FTSR |= (1U << 12) |
                  (1U << 13) |
                  (1U << 14) |
                  (1U << 15);

    /* Disable rising-edge trigger */
    EXTI->RTSR &= ~((1U << 12) |
                    (1U << 13) |
                    (1U << 14) |
                    (1U << 15));

    /* Clear any pending EXTI interrupts */
    EXTI->PR = (1U << 12) |
               (1U << 13) |
               (1U << 14) |
               (1U << 15);

    /* Enable shared EXTI15_10 interrupt in NVIC */
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}