#include <vega/hal.h>
#include <vega/aries.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MILLIS(ms) ((uint32_t)((ms) * 100000UL))

static volatile uint8_t sec_flag = 0;
static volatile int seconds_counter = 0;

int main(void)
{
    char buf[50];

    // Task 1: Initialize UART0
    UART_Init(&huart0);
    UART_Transmit(&huart0, "Timer Seconds Counter Started:\r\n", 32);

    // Task 1: Configure Timer 0 for a periodic 1-second interrupt
    htimer0.LoadCount = MILLIS(1000);
    TIMER_Init(&htimer0);
    TIMER_Start_IT(&htimer0);
    __enable_irq();

    while (1) {
        // Task 3: Print the current count from main() when flagged by the ISR
        if (sec_flag) {
            sec_flag = 0; 

            // Task 4: When count reaches 10, reset to 0
            if (seconds_counter > 10) {
                seconds_counter = 0;
            }

            // Print count via UART
            sprintf(buf, "Elapsed Seconds: %d\r\n", seconds_counter);
            UART_Transmit(&huart0, buf, strlen(buf));
        }
    }

    return 0;
}

// Task 2 & 5: Timer Callback 
void TIMER_ElapsedCallback(TIMER_Handle_t *htimer)
{
    if (htimer == &htimer0) {
        seconds_counter++;
        sec_flag = 1; 
    }
}