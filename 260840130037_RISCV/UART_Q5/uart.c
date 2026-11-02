#include <vega/hal.h>
#include <stdio.h>
#include <string.h>

int main()
{
    char rxdata;
    char buffer[32]; 
    int index = 0;
    char out_msg[100];

    // Task 1: Initialize UART0
    UART_Init(&huart0);

    while (1) {
        // Task 1: Prompt user
        UART_Transmit(&huart0, "Enter text (max 30 chars): \r\n", 31);
        index = 0;
        memset(buffer, 0, sizeof(buffer));

        // Task 2: Receive characters until Enter is pressed
        while (1) {
            UART_Receive(&huart0, &rxdata, 1);

            UART_Transmit(&huart0, &rxdata, 1);

            if (rxdata == '\r' || rxdata == '\n') {
                buffer[index] = '\0'; 
                UART_Transmit(&huart0, "\r\n", 2);
                break;
            }

            // Task 5: Handle buffer bounds (max 30 characters)
            if (index < 30) {
                buffer[index++] = rxdata;
            } else {
                buffer[30] = '\0';
                UART_Transmit(&huart0, "\r\n[Note: Max 30 chars exceeded, truncated]\r\n", 43);
                break;
            }
        }

        // Task 3: Count vowels (a, e, i, o, u in uppercase or lowercase)
        int vowel_count = 0;
        for (int i = 0; i < index; i++) {
            char c = buffer[i];
            if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
                c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U') {
                vowel_count++;
            }
        }

        // Task 4: Print original message, length, and vowel count
        sprintf(out_msg, "Message: %s\r\nLength: %d\r\nVowels: %d\r\n--------------------\r\n", 
                buffer, index, vowel_count);
        UART_Transmit(&huart0, out_msg, strlen(out_msg));
    }

    return 0;
}