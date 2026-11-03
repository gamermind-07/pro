```c
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart2;

volatile uint8_t buttonPressed = 0;

uint8_t tx[2];
uint8_t rx[6];
uint8_t whoami;

int16_t accel_x, accel_y, accel_z;

char uartBuffer[100];

#define CS_PORT GPIOE
#define CS_PIN  GPIO_PIN_3

#define LED_PORT GPIOD
#define LED1 GPIO_PIN_12
#define LED2 GPIO_PIN_13
#define LED3 GPIO_PIN_14
#define LED4 GPIO_PIN_15

#define TILT_THRESHOLD 5000

void LIS3DSH_Write(uint8_t reg, uint8_t data)
{
    uint8_t buffer[2];

    buffer[0] = reg & 0x3F;
    buffer[1] = data;

    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, buffer, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
}

void LIS3DSH_ReadXYZ(void)
{
    uint8_t address = 0x28 | 0xC0;

    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);

    HAL_SPI_Transmit(&hspi1, &address, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(&hspi1, rx, 6, HAL_MAX_DELAY);

    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);

    accel_x = (int16_t)((rx[1] << 8) | rx[0]);
    accel_y = (int16_t)((rx[3] << 8) | rx[2]);
    accel_z = (int16_t)((rx[5] << 8) | rx[4]);
}

void LED_All_Off(void)
{
    HAL_GPIO_WritePin(LED_PORT,
                      LED1 | LED2 | LED3 | LED4,
                      GPIO_PIN_RESET);
}

void Detect_Tilt(void)
{
    LED_All_Off();

    if (abs(accel_x) > TILT_THRESHOLD)
    {
        if (accel_x > 0)
            HAL_GPIO_WritePin(LED_PORT, LED1, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(LED_PORT, LED2, GPIO_PIN_SET);
    }
    else if (abs(accel_y) > TILT_THRESHOLD)
    {
        if (accel_y > 0)
            HAL_GPIO_WritePin(LED_PORT, LED3, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(LED_PORT, LED4, GPIO_PIN_SET);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_0)
    {
        buttonPressed = 1;
    }
}

int main(void)
{
    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();
    MX_SPI1_Init();
    MX_USART2_UART_Init();

    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);

    // Check LIS3DSH identity
    uint8_t address = 0x8F;

    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &address, 1, HAL_MAX_DELAY);
    HAL_SPI_Receive(&hspi1, &whoami, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);

    // Configure LIS3DSH: XYZ enabled, 100 Hz, high resolution
    LIS3DSH_Write(0x20, 0x37);

    while (1)
    {
        LIS3DSH_ReadXYZ();

        Detect_Tilt();

        if (buttonPressed)
        {
            buttonPressed = 0;

            sprintf(uartBuffer,
                    "X: %d Y: %d Z: %d\r\n",
                    accel_x, accel_y, accel_z);

            HAL_UART_Transmit(&huart2,
                              (uint8_t *)uartBuffer,
                              strlen(uartBuffer),
                              HAL_MAX_DELAY);
        }

        HAL_Delay(100);
    }
}
```
