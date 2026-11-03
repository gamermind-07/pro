#include <vega/hal.h>
#include <vega/aries.h>
#include <stdint.h>
#include <stdio.h>

#define TIMER_CLOCK 100000000UL
#define TIMER_1_SEC TIMER_CLOCK

static char received_char;

static volatile char command;
static volatile uint8_t command_ready = 0;

static volatile uint8_t running = 0;
static volatile uint32_t seconds = 0;

static uint32_t displayed_seconds = 0;


int main(void)
{
    char message[50];
    int length;

    // Initialize UART
    UART_Init(&huart0);

    // Configure timer for 1 second
    htimer0.LoadCount = TIMER_1_SEC;

    TIMER_Init(&htimer0);

    // Enable UART reception interrupt
    UART_Receive_IT(&huart0, &received_char, 1U);

    // Display initial message
    length = sprintf(message,
                     "\r\nDigital Stopwatch Ready\r\n"
                     "S = Start\r\n"
                     "E = Stop\r\n"
                     "R = Reset\r\n");

    UART_Transmit(&huart0, message, (uint32_t)length);

    while (1)
    {
        // Check received UART command
        if (command_ready)
        {
            char current_command = command;

            command_ready = 0;

            // START
            if (current_command == 'S' && running == 0)
            {
                if (seconds < 60)
                {
                    running = 1;

                    TIMER_Start_IT(&htimer0);

                    UART_Transmit(&huart0,
                                  "\r\nStopwatch Started\r\n",
                                  22U);
                }
            }

            // STOP
            else if (current_command == 'E' && running == 1)
            {
                running = 0;

                TIMER_Stop_IT(&htimer0);

                UART_Transmit(&huart0,
                              "\r\nStopwatch Stopped\r\n",
                              22U);
            }

            // RESET
            else if (current_command == 'R')
            {
                running = 0;

                TIMER_Stop_IT(&htimer0);

                seconds = 0;
                displayed_seconds = 0;

                UART_Transmit(&huart0,
                              "\r\nStopwatch Reset\r\n",
                              20U);
            }
        }

        // Display updated time
        if (seconds != displayed_seconds)
        {
            displayed_seconds = seconds;

            length = sprintf(message,
                             "Time: %02lu seconds\r\n",
                             (unsigned long)seconds);

            UART_Transmit(&huart0,
                          message,
                          (uint32_t)length);
        }
    }

    return 0;
}


// UART Receive Interrupt Callback
void UART_RxCpltCallback(UART_Handle_t *huart)
{
    if (huart == &huart0)
    {
        command = received_char;

        command_ready = 1;

        // Enable reception for next character
        UART_Receive_IT(&huart0, &received_char, 1U);
    }
}


// Timer Interrupt Callback
void TIMER_ElapsedCallback(TIMER_Handle_t *htimer)
{
    if (htimer == &htimer0 && running == 1)
    {
        seconds++;

        // Stop automatically after 60 seconds
        if (seconds >= 60)
        {
            running = 0;

            TIMER_Stop_IT(&htimer0);
        }
    }
}