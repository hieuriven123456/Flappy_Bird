#include "soft_i2c.h"

static void I2C_Delay(void) {
    for (volatile int i = 0; i < 3; i++); // 
}

static void SDA_High(void) {
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SDA_PIN, GPIO_PIN_SET);
}
static void SDA_Low(void) {
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SDA_PIN, GPIO_PIN_RESET);
}
static void SCL_High(void) {
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_SET);
    I2C_Delay();
}
static void SCL_Low(void) {
    HAL_GPIO_WritePin(SOFT_I2C_PORT, SOFT_I2C_SCL_PIN, GPIO_PIN_RESET);
    I2C_Delay();
}

// Cấu hình GPIO open-drain
void SoftI2C_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = SOFT_I2C_SDA_PIN | SOFT_I2C_SCL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SOFT_I2C_PORT, &GPIO_InitStruct);

    SDA_High();
    SCL_High();
}

void SoftI2C_Start(void) {
    SDA_High(); SCL_High(); I2C_Delay();
    SDA_Low(); I2C_Delay();
    SCL_Low(); I2C_Delay();
}

void SoftI2C_Stop(void) {
    SDA_Low(); SCL_High(); I2C_Delay();
    SDA_High(); I2C_Delay();
}

static uint8_t SoftI2C_WriteBit(uint8_t bit) {
    if (bit) SDA_High(); else SDA_Low();
    SCL_High();
    SCL_Low();
    return 0;
}

static uint8_t SoftI2C_ReadBit(void) {
    SDA_High(); // Thả SDA để đọc (open-drain với pull-up)
    SCL_High();
    uint8_t bit = HAL_GPIO_ReadPin(SOFT_I2C_PORT, SOFT_I2C_SDA_PIN);
    SCL_Low();
    return bit;
}

uint8_t SoftI2C_WriteByte(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        SoftI2C_WriteBit((data & 0x80) != 0);
        data <<= 1;
    }
    return SoftI2C_ReadBit() == 0; // ACK = 0
}

uint8_t SoftI2C_ReadByte(uint8_t ack) {
    uint8_t data = 0;
    for (int i = 0; i < 8; i++) {
        data <<= 1;
        data |= SoftI2C_ReadBit();
    }
    SoftI2C_WriteBit(!ack); // gửi ACK (0) hoặc NACK (1)
    return data;
}
