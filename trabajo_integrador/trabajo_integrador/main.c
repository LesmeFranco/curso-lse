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
    xTaskCreate(tsk_display_write, "Display", tskDisplayWrite_STACK, NULL, tskDisplayWrite_PRIORITY, NULL);
    xTaskCreate(tsk_buzzer, "Buzzer", tskBuzzer_STACK, NULL, tskBuzzer_PRIORITY, NULL);
    xTaskCreate(tsk_pwm, "pwm", tskPwm_STACK, NULL, tskPwm_PRIORITY, NULL);
    xTaskCreate(tsk_display_change, "boton", tskDisplayChange_STACK, NULL, tskDisplayChange_PRIORITY, NULL);
    xTaskCreate(tsk_control, "Counter", tskControl_STACK, NULL, tskControl_PRIORITY, NULL);

    vTaskStartScheduler();
}