#ifndef __SOFT_I2C_H
#define __SOFT_I2C_H

#include "stm32l1xx_hal.h"   // STM32L151C8T6

// Định nghĩa chân SDA và SCL
#define SOFT_I2C_PORT GPIOB
#define SOFT_I2C_SDA_PIN GPIO_PIN_12
#define SOFT_I2C_SCL_PIN GPIO_PIN_13

// Hàm khởi tạo
void SoftI2C_Init(void);

// Thao tác I2C
void SoftI2C_Start(void);
void SoftI2C_Stop(void);
uint8_t SoftI2C_WriteByte(uint8_t data);
uint8_t SoftI2C_ReadByte(uint8_t ack);

#endif
