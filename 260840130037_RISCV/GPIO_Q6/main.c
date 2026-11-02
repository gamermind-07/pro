#include <vega/hal.h>
#include <vega/aries.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    char buf[50];
    int button_count = 0;
    uint8_t last_button_state = 1; 
    uint8_t current_state;

    UART_Init(&huart0);
    GPIO_Init(GPIOB, RGB_GREEN, GPIO_OUT);
    RGB_GREEN_RST;

    GPIO_Init(GPIOB, BTN0, GPIO_IN);

    UART_Transmit(&huart0, "Button Press Counter Started.\r\n", 31);

    while (1) {
        current_state = GPIO_ReadPin(GPIOB, BTN0);

        if (last_button_state == 1 && current_state == 0) {
            // Task 3: Software debounce (~50 ms)
            delayms(50);


            if (GPIO_ReadPin(GPIOB, BTN0) == 0) {
                // Task 2: Increase counter by 1 on each new press
                button_count++;

                // Task 6: Wrap back to zero after count 20
                if (button_count > 20) {
                    button_count = 0;
                }

                // Task 4: Print "Button count: N" to UART
                sprintf(buf, "Button count: %d\r\n", button_count);
                UART_Transmit(&huart0, buf, strlen(buf));

                // Task 5: On every fifth press, flash the green LED twice
                if (button_count > 0 && (button_count % 5 == 0)) {
                    for (int i = 0; i < 2; i++) {
                        RGB_GREEN_SET;
                        delayms(100);
                        RGB_GREEN_RST;
                        delayms(100);
                    }
                }

                while (GPIO_ReadPin(GPIOB, BTN0) == 0) {
                }
            }
        }

        last_button_state = current_state;
    }

    return 0;
}