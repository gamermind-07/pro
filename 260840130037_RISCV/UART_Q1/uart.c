#include <vega/hal.h>
#include <stdio.h>
#include <string.h>

int main()
{
    char buf[80];
    char rx_data;
    int counter = 0;

    // Task 1: Initialize UART0
    UART_Init(&huart0);
    UART_Transmit(&huart0, "Counter begin (0 - 20)\r\n", 24);

    while(1) {
        // Task 2: Receive one character at a time using UART polling
        UART_Receive(&huart0, &rx_data, 1);

        // Task 6: Ignore Enter (CR/LF) characters
        if(rx_data == '\r' || rx_data == '\n') {
            continue;
        }
        
        // Task 3: Command handling
        if(rx_data == '+') {
            if(counter < 20) {
                counter++;
            } 
            else {
                UART_Transmit(&huart0, "LIMIT REACHED\r\n", 15);
                continue; 
            }
        }
        else if(rx_data == '-') {
            if(counter > 0) {
                counter--;
            } 
            else {
                UART_Transmit(&huart0, "LIMIT REACHED\r\n", 15);
                continue; 
            }
        }
        else if(rx_data == 'r' || rx_data == 'R') {
            counter = 0;
        }    
        else {
            continue; 
        }

        // Task 4: After every valid command, print the updated counter value
        sprintf(buf, "counter_value : %d\r\n", counter);
        UART_Transmit(&huart0, buf, strlen(buf));
    }
    
    return 0;
}