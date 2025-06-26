#include "board.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "wrappers.h"
#include "isr.h"

#ifndef _APP_TASKS_H_
#define _APP_TASKS_H_

// Prioridad de las tareas
#define tskINIT_PRIORITY (tskIDLE_PRIORITY + 3UL) // UL (unsigned long) para evitar warnings
#define tskADC_PRIORITY (tskIDLE_PRIORITY + 2UL)
#define tskBH1750_PRIORITY (tskIDLE_PRIORITY + 1UL)
#define tskLEDS_PRIORITY (tskIDLE_PRIORITY + 3UL) // Tarea de setpoint
#define tskDisplay_PRIORITY (tskIDLE_PRIORITY + 2UL)
#define tskIntensity_PRIORITY (tskIDLE_PRIORITY + 1UL) // Tarea de RV22
#define tskBuzzer_PRIORITY (tskIDLE_PRIORITY + 3UL)    // Tarea del Sensor IR
#define tskTerminal_PRIORITY (tskIDLE_PRIORITY + 1UL)

// Stack size de las tareas (Tamaño de la pila de cada tarea)
#define tskINIT_STACK (3 * configMINIMAL_STACK_SIZE)
#define tskADC_STACK (3 * configMINIMAL_STACK_SIZE)
#define tskBH1750_STACK (3 * configMINIMAL_STACK_SIZE)
#define tskLEDS_STACK (3 * configMINIMAL_STACK_SIZE)
#define tskDisplay_STACK (3 * configMINIMAL_STACK_SIZE)
#define tskIntensity_STACK (3 * configMINIMAL_STACK_SIZE)
#define tskBuzzer_STACK (3 * configMINIMAL_STACK_SIZE)
#define tskTerminal_STACK (3 * configMINIMAL_STACK_SIZE)

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
void tsk_pwm(void *params);
void tsk_terminal(void *params);