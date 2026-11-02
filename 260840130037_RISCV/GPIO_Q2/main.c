#include <vega/hal.h>
#include <vega/aries.h>

int main(void)
{
    GPIO_Init(GPIOB, RGB_RED, GPIO_OUT);
    RGB_RED_RST;

    GPIO_Init(GPIOB, BTN0, GPIO_IN);

    while (1) {
        if (GPIO_ReadPin(GPIOB, BTN0) == 0) {
        
            delayms(20); 
            if (GPIO_ReadPin(GPIOB, BTN0) == 0) {
                RGB_RED_SET;
                delayms(125);
                RGB_RED_RST;
                delayms(125);
                continue; 
            }
        }
        // Task 1 & 3: Normal mode - blink red LED every 1000 ms (500ms ON, 500ms OFF)
        RGB_RED_SET;
        delayms(500);
        RGB_RED_RST;
        delayms(500);
    }

    return 0;
}