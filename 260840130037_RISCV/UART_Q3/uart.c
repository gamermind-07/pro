#include <vega/hal.h>
#include <stdio.h>
#include <string.h>

int main()
{
    char rxdata;
    char buffer[20];
    int index = 0;
    char out_msg[50];

    // Task 1: Initialize UART0
    UART_Init(&huart0);
    UART_Transmit(&huart0, "Calculator Ready (e.g., 7+2):\r\n", 31);

    while (1) {
        // Task 2 & 3: Receive characters via polling
        UART_Receive(&huart0, &rxdata, 1);

        UART_Transmit(&huart0, &rxdata, 1);

        // Check if Enter key was pressed (Task 2)
        if (rxdata == '\r' || rxdata == '\n') {
            UART_Transmit(&huart0, "\r\n", 2);

            // Task 5: Validate expression length (must be exactly 3 characters: digit, op, digit)
            if (index == 3 && 
                buffer[0] >= '0' && buffer[0] <= '9' &&
                buffer[2] >= '0' && buffer[2] <= '9' &&
                (buffer[1] == '+' || buffer[1] == '-')) 
            {
                // Convert ASCII characters to integers
                int num1 = buffer[0] - '0';
                int num2 = buffer[2] - '0';
                int result = 0;

                // Task 3 & 4: Compute answer and handle negative subtraction correctly
                if (buffer[1] == '+') {
                    result = num1 + num2;
                } else if (buffer[1] == '-') {
                    result = num1 - num2;
                }

                // Print result through UART
                sprintf(out_msg, "Result: %d\r\n", result);
                UART_Transmit(&huart0, out_msg, strlen(out_msg));
            } 
            else {
                // Task 5: Print error for invalid inputs
                UART_Transmit(&huart0, "Invalid expression\r\n", 20);
            }

            // Reset buffer index for the next expression (Task 6)
            index = 0;
            UART_Transmit(&huart0, "Enter expr: ", 12);
        } 
        else {

            if (index < sizeof(buffer) - 1) {
                buffer[index++] = rxdata;
            }
        }
    }

    return 0;
}