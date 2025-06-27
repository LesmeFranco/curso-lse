#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "wrappers.h"
#include "isr.h"

#ifndef _APP_TASKS_H
#define _APP_TASKS_H

// Prioridad de las tareas
#define tskINIT_PRIORITY (tskIDLE_PRIORITY + 3UL) // UL (unsigned long) para evitar warnings
#define tskADC_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define tskBH1750_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define tskLEDS_PRIORITY (tskIDLE_PRIORITY + 2UL) // Tarea de setpoint
#define tskDisplayChange_PRIORITY (tskIDLE_PRIORITY + 2UL)
#define tskControl_PRIORITY (tskIDLE_PRIORITY + 1UL)	  // Tarea de control
#define tskDisplayWrite_PRIORITY (tskIDLE_PRIORITY + 1UL) // Tarea de display write
#define tskIntensity_PRIORITY (tskIDLE_PRIORITY + 1UL)	  // Tarea de RV22
#define tskBuzzer_PRIORITY (tskIDLE_PRIORITY + 2UL)		  // Tarea del Sensor IR
#define tskTerminal_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define tskPwm_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define tskCounter_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define tskCOUNTER_BTNS_PRIORITY (tskIDLE_PRIORITY + 2UL)

// Stack size de las tareas (Tamaño de la pila de cada tarea)
#define tskINIT_STACK (3 * configMINIMAL_STACK_SIZE)
#define tskADC_STACK (configMINIMAL_STACK_SIZE)
#define tskBH1750_STACK (configMINIMAL_STACK_SIZE)
#define tskLEDS_STACK (configMINIMAL_STACK_SIZE)
#define tskDisplayChange_STACK (configMINIMAL_STACK_SIZE)
#define tskDisplayWrite_STACK (3 * configMINIMAL_STACK_SIZE / 2)
#define tskControl_STACK (configMINIMAL_STACK_SIZE)
#define tskIntensity_STACK (configMINIMAL_STACK_SIZE)
#define tskBuzzer_STACK (configMINIMAL_STACK_SIZE)
#define tskTerminal_STACK (2 * configMINIMAL_STACK_SIZE)
#define tskPwm_STACK (3 * configMINIMAL_STACK_SIZE / 2)
#define tskCounter_STACK (configMINIMAL_STACK_SIZE)
#define tskCOUNTER_BTNS_STACK (configMINIMAL_STACK_SIZE)

// Handler para la tarea de display write
extern TaskHandle_t handle_display;

// Prototipos de las tareas
void tsk_init(void *params);
void tsk_adc(void *params);
void tsk_display_change(void *params);
void tsk_control(void *params);
void tsk_display_write(void *params);
void tsk_BH1750(void *params);
void tsk_LEDS(void *params);
void tsk_buzzer(void *params);
void tsk_counter(void *params);
void tsk_counter_btns(void *params);
void tsk_pwm(void *params);
// void tsk_terminal(void *params);

/**
 * @brief Wrapper que verifica el estado de un pulsador con pull-up
 * aplicando un antirebote
 * @param btn estructura al GPIO del pulsador
 */
static inline bool wrapper_btn_get_with_debouncing_with_pull_up(gpio_t btn)
{
	//	Pregunto si se precionó el pulsador
	if (!wrapper_btn_get(btn))
	{
		// Antirebote
		vTaskDelay(pdMS_TO_TICKS(20));
		if (!wrapper_btn_get(btn))
		{
			return true;
		}
	}
	return false;
}

#endif