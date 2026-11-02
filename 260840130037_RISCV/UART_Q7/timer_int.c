#include <vega/hal.h>
#include <vega/aries.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MILLIS(ms) ((uint32_t)((ms) * 100000UL))

static volatile uint8_t timer_tick_flag = 0;
static volatile char rxdata = 0;
static volatile uint8_t rx_ready = 0;
static int elapsed_seconds = 0;
static uint8_t led_is_on = 0;

int main(void)
{
    char buf[50];

    UART_Init(&huart0);
    GPIO_Init(GPIOB, RGB_BLUE, GPIO_OUT);
    RGB_BLUE_RST;

    htimer0.LoadCount = MILLIS(1000);
    TIMER_Init(&htimer0);
    TIMER_Start_IT(&htimer0);

    UART_Receive_IT(&huart0, (char *)&rxdata, 1);
    __enable_irq();

    UART_Transmit(&huart0, "System Ready. Press 'o' for ON/Reset, 's' for Status.\r\n", 57);

    while (1) {
        if (rx_ready) {
            rx_ready = 0; 

            UART_Transmit(&huart0, (char *)&rxdata, 1);
            UART_Transmit(&huart0, "\r\n", 2);

            if (rxdata == 'o' || rxdata == 'O') {
                RGB_BLUE_SET;
                led_is_on = 1;
                elapsed_seconds = 0;
                UART_Transmit(&huart0, "LED ON. 5s timer started.\r\n", 27);
            }
            else if (rxdata == 's' || rxdata == 'S') {
                if (led_is_on) {
                    sprintf(buf, "Status: LED is ON (%d sec elapsed)\r\n", elapsed_seconds);
                } else {
                    sprintf(buf, "Status: LED is OFF\r\n");
                }
                UART_Transmit(&huart0, buf, strlen(buf));
            }

            UART_Receive_IT(&huart0, (char *)&rxdata, 1);
        }

        if (timer_tick_flag) {
            timer_tick_flag = 0;

            if (led_is_on) {
                elapsed_seconds++;
                if (elapsed_seconds >= 5) {
                    RGB_BLUE_RST;
                    led_is_on = 0;
                    elapsed_seconds = 0;
                    UART_Transmit(&huart0, "Time elapsed: LED turned OFF automatically.\r\n", 45);
                }
            }
        }
    }
    return 0;
}

void TIMER_ElapsedCallback(TIMER_Handle_t *htimer)
{
    if (htimer == &htimer0) {
        timer_tick_flag = 1;
    }
}

void UART_RxCpltCallback(UART_Handle_t *huart)
{
    if (huart == &huart0) {
        rx_ready = 1; 
    }
}