#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include "stm32f103xb.h"

/*==========================================================
 * I2C Direction
 *=========================================================*/
#define I2C_WRITE     0U
#define I2C_READ      1U

/*==========================================================
 * Low Level Functions
 *=========================================================*/
void I2C1_Init(void);

void I2C1_Start(void);

void I2C1_Stop(void);

void I2C1_SendAddress(uint8_t address,
                      uint8_t direction);

void I2C1_SendByte(uint8_t data);

uint8_t I2C1_ReadByte_ACK(void);

uint8_t I2C1_ReadByte_NACK(void);

/*==========================================================
 * High Level Functions
 *=========================================================*/
void I2C1_Write(uint8_t slave_add,
                uint8_t register_add,
                uint8_t data);

uint8_t I2C1_Read(uint8_t slave_add,
                  uint8_t register_add);

void I2C1_WriteBuffer(uint8_t slaveAddr,
                      uint8_t regAddr,
                      uint8_t *buffer,
                      uint16_t length);

void I2C1_ReadBuffer(uint8_t slaveAddr,
                     uint8_t regAddr,
                     uint8_t *buffer,
                     uint16_t length);

#endif