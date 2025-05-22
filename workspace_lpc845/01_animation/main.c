#include <stdint.h>
#include <stdio.h>
#include "board.h"

// Mapeo de segmentos: A, B, C, D, E, F, G
//      --A--
//     |     |
//     F     B
//     |     |
//      --G--
//     |     |
//     E     C
//     |     |
//      --D--

#define LED_A GPIO, 0, 11
#define LED_B GPIO, 0, 10
#define LED_C GPIO, 0, 6
#define LED_D GPIO, 0, 14
#define LED_E GPIO, 0, 0
#define LED_F GPIO, 0, 13
#define LED_G GPIO, 0, 15

// Cada bit representa un segmento
const uint8_t digit_to_segments[10] = {
    0b00111111, // 0: A B C D E F
    0b00000110, // 1: B C
    0b01011011, // 2: A B D E
    0b01001111, // 3: A B C D G
    0b01100110, // 4: B C G
    0b01101101, // 5: A C D F G
    0b01111101, // 6: A C D E F G
    0b00000111, // 7: A B
    0b01111111, // 8: A B C D E F G
    0b01101111  // 9: A B C D F G
};

void display_digit(int digit)
{
    if (digit < 0 || digit > 9)
        return;
    uint8_t seg = digit_to_segments[digit];
    printf("Mostrando el dígito %d en 7 segmentos: 0x%02X\n", digit, seg);

    // Encender los pines correspondientes
    GPIO_PortInit(GPIO, 0);
    gpio_pin_config_t out_config = {.pinDirection = kGPIO_DigitalOutput, .outputLogic = 1};

    // Inicializo Pin como Salida
    GPIO_PinInit(GPIO, 0, 11, &out_config);
    GPIO_PinInit(GPIO, 0, 10, &out_config);
    GPIO_PinInit(GPIO, 0, 6, &out_config);
    GPIO_PinInit(GPIO, 0, 14, &out_config);
    GPIO_PinInit(GPIO, 0, 0, &out_config);
    GPIO_PinInit(GPIO, 0, 13, &out_config);
    GPIO_PinInit(GPIO, 0, 15, &out_config);

    while (1)
    {
        GPIO_PinWrite(LED_A, 1);
        GPIO_PinWrite(LED_B, 1);
        GPIO_PinWrite(LED_C, 1);
        GPIO_PinWrite(LED_D, 1);
        GPIO_PinWrite(LED_E, 1);
        GPIO_PinWrite(LED_F, 1);
        GPIO_PinWrite(LED_G, 1);
    }

    return;
}

int main(void)
{
    for (int i = 0; i < 10; i++)
    {
        display_digit(i);
    }
    while (1)
        ; // Bucle infinito para que el programa no termine
}