// ------------------------------------
//         Trabajo Integrador         |
// ------------------------------------

#include "board.h"
#include "FreeRTOS.h"
#include "task.h"

#include "tareas.h"

int main(void)
{

    //  Clock a 30MHz
    BOARD_BootClockFRO30M();

    // Creo tareas
    xTaskCreate(tsk_init, "Init", tskINIT_STACK, NULL, tskINIT_PRIORITY, NULL);
    xTaskCreate(tsk_adc, "ADC", tskADC_STACK, NULL, tskADC_PRIORITY, NULL);
    xTaskCreate(tsk_BH1750, "BH1750", tskBH1750_STACK, NULL, tskBH1750_PRIORITY, NULL);
    xTaskCreate(tsk_LEDS, "LEDS", tskLEDS_STACK, NULL, tskLEDS_PRIORITY, NULL);
    xTaskCreate(tsk_display_write, "Display", tskDisplay_STACK, NULL, tskDisplay_PRIORITY, NULL);
    xTaskCreate(tsk_buzzer, "Buzzer", tskBuzzer_STACK, NULL, tskBuzzer_PRIORITY, NULL);
    xTaskCreate(tsk_terminal, "Terminal", tskTerminal_STACK, NULL, tskTerminal_PRIORITY, NULL);
    xTaskCreate(tsk_pwm, "pwm", tskTerminal_STACK, NULL, tskTerminal_PRIORITY, NULL);
    xTaskCreate(tsk_display_change, "boton", tskTerminal_STACK, NULL, tskTerminal_PRIORITY, NULL);
    xTaskCreate(tsk_counter, "Contador", tskTerminal_STACK, NULL, tskTerminal_PRIORITY, NULL);
    xTaskCreate(tsk_control, "Write", tskTerminal_STACK, NULL, tskTerminal_PRIORITY, NULL);

    vTaskStartScheduler();
}