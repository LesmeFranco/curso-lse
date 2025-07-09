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
    BOARD_InitDebugConsole();

    // Creo tareas
    xTaskCreate(tsk_init, "Init", tskINIT_STACK, NULL, tskINIT_PRIORITY, NULL);
    xTaskCreate(tsk_adc, "ADC", tskADC_STACK, NULL, tskADC_PRIORITY, NULL);
    xTaskCreate(tsk_BH1750, "BH1750", tskBH1750_STACK, NULL, tskBH1750_PRIORITY, NULL);
    xTaskCreate(tsk_setpoint, "Setpoint", tsk_setpoint_STACK, NULL, tsk_setpoint_PRIORITY, NULL);
    xTaskCreate(tsk_display_write, "Display", tskDisplayWrite_STACK, NULL, tskDisplayWrite_PRIORITY, NULL);
    xTaskCreate(tsk_display_change, "boton", tskDisplayChange_STACK, NULL, tskDisplayChange_PRIORITY, NULL);
    xTaskCreate(tsk_control, "Counter", tskControl_STACK, NULL, tskControl_PRIORITY, NULL);
    xTaskCreate(tsk_buzzer, "Buzzer", tskBuzzer_STACK, NULL, tskBuzzer_PRIORITY, NULL);
    xTaskCreate(tsk_led_azul, "LED_Azul", tskLED_Azul_STACK, NULL, tskLED_Azul_PRIORITY, NULL);
    xTaskCreate(tsk_leds_control, "LEDs", tsk_leds_control_STACK, NULL, tsk_leds_control_PRIORITY, NULL);
    xTaskCreate(tsk_console_monitor, "Console", tsk_console_monitor_STACK, NULL, tsk_console_monitor_PRIORITY, NULL);

    vTaskStartScheduler();
}