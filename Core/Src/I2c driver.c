#include "stm32f103xb.h"
#include "I2C driver.h"

void I2C1_Init(void);
void I2C1_Start(void);
void I2C1_Stop(void);
void I2C1_SendAddress(uint8_t address, uint8_t direction);
void I2C1_SendByte(uint8_t data);
uint8_t I2C1_ReadByte_ACK(void);
uint8_t I2C1_ReadByte_NACK(void);
void I2C1_Write(uint8_t slave_add,uint8_t register_add,uint8_t data);
uint8_t I2C1_Read(uint8_t slave_add,uint8_t register_add);
void I2C1_ReadBuffer(uint8_t slaveAddr,uint8_t regAddr,uint8_t *buffer,uint16_t length);
void I2C1_WriteBuffer(uint8_t slaveAddr, uint8_t regAddr,uint8_t *buffer,uint16_t length);




void I2C1_Init(void)
{
    // Enable the clock for I2C1
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    GPIOB->CRL &= ~(0xFF << 24);
    GPIOB->CRL |=  (0xFF << 24);

    /*======================================================
     * Reset I2C Peripheral
     *=====================================================*/

    I2C1->CR1 |= I2C_CR1_SWRST;
    I2C1->CR1 &= ~I2C_CR1_SWRST;

    /* APB1 Clock = 8 MHz */
    I2C1->CR2 &= ~I2C_CR2_FREQ;
    I2C1->CR2 |= 8;

    /****************************************************/

    /*======================================================
     * CCR
     * Standard Mode
     * SCL = 100 kHz
     * CCR = FPCLK1 / (2 × FSCL)
     * CCR = 8MHz / (2 × 100kHz)
     * CCR = 40
     *=====================================================*/

    I2C1->CCR = 8000000 / (2U * 100000U); 
    /*======================================================
     * TRISE
     * Standard Mode
     *
     * TRISE = FPCLK1(MHz) + 1
     * TRISE = 9
     *=====================================================*/

    I2C1->TRISE = 8 + 1U;

    // I2C1->CR1 |= I2C_CR1_ACK;   

    I2C1->CR1 |= I2C_CR1_PE;

}

void I2C1_Start(void)
{
    /*----------------------------------------------------------
     * Generate START Condition
     *---------------------------------------------------------*/
    I2C1->CR1 |= I2C_CR1_START;

    /*----------------------------------------------------------
     * Wait until START condition generated
     * (SB bit in SR1 becomes 1)
     *---------------------------------------------------------*/
    uint32_t timeout = 100000;

    while (!(I2C1->SR1 & I2C_SR1_SB))
    {
        if (--timeout == 0)
            return ;
    }
}

void I2C1_Stop(void)
{
    /*----------------------------------------------------------
     * Generate STOP Condition
     *---------------------------------------------------------*/
    I2C1->CR1 |= I2C_CR1_STOP;
}

void I2C1_SendAddress(uint8_t address, uint8_t direction)
{
    /*----------------------------------------------------------
     * Send Address
     *---------------------------------------------------------*/
    I2C1->DR = (address << 1) | direction;
    uint32_t timeout = 100000;
    /* Wait until address acknowledged */
    while(!(I2C1->SR1 & I2C_SR1_ADDR))
    {
        if (--timeout == 0)
        return ;
    }
    (void)I2C1->SR1; // Clear ADDR flag by reading SR1
    (void)I2C1->SR2; // Clear ADDR flag by reading SR2
}

void I2C1_SendByte(uint8_t data)
{
    uint32_t timeout = 100000;
    while (!(I2C1->SR1 & I2C_SR1_TXE))
    {
        if (--timeout == 0)
        return ;
    }
    I2C1->DR = data;

    while (!(I2C1->SR1 & I2C_SR1_BTF))
    {
        if (--timeout == 0)
        return ;
    }
}

uint8_t I2C1_ReadByte_ACK(void)
{
    uint32_t timeout = 100000;
    /* Enable ACK */
    I2C1->CR1 |= I2C_CR1_ACK;

    /* Wait until data received */
    while(!(I2C1->SR1 & I2C_SR1_RXNE))
    {
        if (--timeout == 0)
        return 0;
    }

    /* Return received byte */
    return (uint8_t)I2C1->DR;
}

uint8_t I2C1_ReadByte_NACK(void)
{
    uint32_t timeout = 100000;
    /* Disable ACK */
    I2C1->CR1 &= ~I2C_CR1_ACK;

    /* Wait until data received */
    while(!(I2C1->SR1 & I2C_SR1_RXNE))
    {
        if (--timeout == 0)
        return 0;
    }

    /* Return received byte */
    return (uint8_t)I2C1->DR;
}

void I2C1_Write(uint8_t slave_add,uint8_t register_add,uint8_t data)
{
    I2C1_Start();

    I2C1_SendAddress(slave_add, 0);  //0 FOR WRITE 

    I2C1_SendByte(register_add);

    I2C1_SendByte(data);

    I2C1_Stop();
}

uint8_t I2C1_Read(uint8_t slave_add,uint8_t register_add)
{
    uint8_t data;

    I2C1_Start();

    I2C1_SendAddress(slave_add,0); // 0 FOR WRITE 

    I2C1_SendByte(register_add);

    I2C1_Start();

    I2C1_SendAddress(slave_add,1);

    data = I2C1_ReadByte_NACK();

    I2C1_Stop();

    return data;
}

void I2C1_WriteBuffer(uint8_t slaveAddr, uint8_t regAddr,uint8_t *buffer,uint16_t length)
{
    I2C1_Start();

    I2C1_SendAddress(slaveAddr, I2C_WRITE);

    I2C1_SendByte(regAddr);

    for(uint16_t i = 0; i < length; i++)
    {
        I2C1_SendByte(buffer[i]);
    }

    I2C1_Stop();
}

void I2C1_ReadBuffer(uint8_t slaveAddr,uint8_t regAddr,uint8_t *buffer,uint16_t length)
{
    I2C1_Start();

    I2C1_SendAddress(slaveAddr, I2C_WRITE);

    I2C1_SendByte(regAddr);

    I2C1_Start();

    I2C1_SendAddress(slaveAddr, I2C_READ);

    for(uint16_t i = 0; i < (length - 1); i++)
    {
        buffer[i] = I2C1_ReadByte_ACK();
    }

    buffer[length - 1] = I2C1_ReadByte_NACK();

    I2C1_Stop();
}