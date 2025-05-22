#include "board.h"

#define LED_BLUE GPIO, 1, 1
#define delay(t) for (uint32_t i = 0; i < t; i++)

int main(void)
{

    // Inicializo Puerto
    GPIO_PortInit(GPIO, 1);
    gpio_pin_config_t out_config = {.pinDirection = kGPIO_DigitalOutput, .outputLogic = 1};

    // Inicializo Pin como Salida
    GPIO_PinInit(GPIO, 1, 1, &out_config);

    while (1)
    {
        GPIO_PinWrite(LED_BLUE, 1);
        delay(150000);
        GPIO_PinWrite(LED_BLUE, 0);
        delay(150000);
    }
    return 0;
}