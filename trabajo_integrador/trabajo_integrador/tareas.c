#include "tareas.h"

// -------
// Colas |
// -------
QueueHandle_t queue_adc;              // Cola para datos del ADC
QueueHandle_t queue_display;          // Cola para datos del display
QueueHandle_t queue_display_variable; // Cola para variable a mostrar en el display
QueueHandle_t queue_lux;              // cola para datos de lux

// ----------
// Semaforo |
// ----------
xSemaphoreHandle semphr_buzz;    // Interrupción del Sensor IR
xSemaphoreHandle semphr_usr;     // Boton de USER
xSemaphoreHandle semphr_counter; // Semáforo para contador
xSemaphoreHandle semphr_mutex;   // Semáforo mutex para el display

// Handler para display (Puntero o referencia para identificar y controlar una tarea específica después de haberla creado)
TaskHandle_t DisplayHandler;

// --------------------------
// Definición de las tareas |
// --------------------------

// Tarea 0: Inicialización de periféricos y recursos
void tsk_init(void *params)
{
    // Inicializo los semaforos
    semphr_buzz = xSemaphoreCreateBinary();
    semphr_usr = xSemaphoreCreateBinary();

    // Incializo colas
    queue_adc = xQueueCreate(1, sizeof(adc_data_t));
    queue_display = xQueueCreate(1, sizeof(uint16_t));
    queue_display_variable = xQueueCreate(1, sizeof(display_variable_t));
    queue_lux = xQueueCreate(1, sizeof(uint16_t));

    // Incialización de GPIO
    wrapper_gpio_init(0);
    wrapper_gpio_init(1);
    // Inicialización del LED
    wrapper_output_init((gpio_t){LED}, true);
    // Inicialización del buzzer
    wrapper_output_init((gpio_t){BUZZER}, false);
    // Inicialización del enable del CNY70
    wrapper_output_init((gpio_t){CNY70_EN}, true);
    // Configuro el ADC
    wrapper_adc_init();
    // Configuro el display
    wrapper_display_init();
    // Configuro botones
    wrapper_btn_init();
    // Configuro interrupción por flancos para el infrarojo y para el botón del user
    wrapper_gpio_enable_irq((gpio_t){CNY70}, kPINT_PinIntEnableBothEdges, cny70_callback);
    wrapper_gpio_enable_irq((gpio_t){USR_BTN}, kPINT_PinIntEnableFallEdge, usr_callback);
    // Inicializo el PWM
    wrapper_pwm_init();
    // Inicializo I2C
    wrapper_i2c_init();
    // Inicializo el BH1750
    wrapper_bh1750_init();

    // Elimino tarea para liberar recursos
    vTaskDelete(NULL);
}

// ---------------
// Tarea 1: ADC  |
// ---------------
void tsk_adc(void *params)
{

    while (1)
    {
        // Inicio una conversion
        ADC_DoSoftwareTriggerConvSeqA(ADC0);
        // Bloqueo la tarea por 250 ms
        vTaskDelay(pdMS_TO_TICKS(250));
    }
}

// ---------------------------------------
// Tarea 2: Control del botón de usuario |
// ---------------------------------------
void tsk_display_change(void *params)
{
    // Dato para pasar
    display_variable_t variable = kDISPLAY_TEMP;

    while (1)
    {
        // Escribe el dato en la cola
        xQueueOverwrite(queue_display_variable, &variable);
        // Intenta tomar el semáforo
        xSemaphoreTake(semphr_usr, portMAX_DELAY);
        // Si se presionó, cambio la variable
        variable = (variable == kDISPLAY_TEMP) ? kDISPLAY_REF : kDISPLAY_TEMP;
    }
}

// --------------------------------
// Tarea 3: Escribe en el display |
// --------------------------------
void tsk_control(void *params)
{
    // Variable a mostrar
    display_variable_t variable = kDISPLAY_TEMP;
    // Valores de ADC
    adc_data_t data = {0};
    // Valor a mostrar
    uint16_t val = 0;

    while (1)
    {
        // Veo que variable hay que mostrar
        xQueuePeek(queue_display_variable, &variable, portMAX_DELAY);
        // Leo los datos del ADC
        xQueuePeek(queue_adc, &data, portMAX_DELAY);
        // Veo cual tengo que mostrar
        val = (variable == kDISPLAY_TEMP) ? data.temp_raw : data.ref_raw;
        val = 30 * val / 4095;
        // Escribo en la cola del display si puedo tomar el mutex
        xSemaphoreTake(semphr_mutex, portMAX_DELAY);
        xQueueOverwrite(queue_display, &val);
        xSemaphoreGive(semphr_mutex);

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// ----------------------------------------
// Tarea 4: Escribe un numero en el display
// ----------------------------------------
void tsk_display_write(void *params)
{
    // Variable con el dato para escribir
    uint16_t data;

    while (1)
    {
        // Mira el dato que haya en la cola
        if (!xQueuePeek(queue_display, &data, pdMS_TO_TICKS(100)))
        {
            continue;
        }
        // Muestro el número
        wrapper_display_off();
        wrapper_display_write((uint8_t)(data / 10));
        wrapper_display_on((gpio_t){COM_1});
        vTaskDelay(pdMS_TO_TICKS(10));
        wrapper_display_off();
        wrapper_display_write((uint8_t)(data % 10));
        wrapper_display_on((gpio_t){COM_2});
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// --------------------------
// Tarea 5: Sensor BH1750
// --------------------------
void tsk_BH1750(void *params)
{
    // Valor de Intensidad de luz
    uint16_t lux = 0;

    while (1)
    {
        // Leo el valor de lux del sensor BH1750
        lux = wrapper_bh1750_read();
        // Muestro por consola
        xQueueOverwrite(queue_lux, &lux);
    }
}

// --------------------------
// Tarea 6: PWM
// --------------------------
void tsk_pwm(void *params)
{
    // Variables para el duty cycle
    int16_t duty_bled = 0;
    int16_t duty_rled = 0;

    while (1)
    {
        // Leo el valor del ADC
        adc_data_t data;
        xQueuePeek(queue_adc, &data, portMAX_DELAY);
        // Actualizo los duty cycles
        duty_bled = (int16_t)(data.temp_raw * 100 / 4095);
        duty_rled = (int16_t)(data.ref_raw * 100 / 4095);
        // Actualizo el PWM
        wrapper_pwm_update_bled(duty_bled);
        wrapper_pwm_update_rled(duty_rled);
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

// --------------------------
// Tarea 7: Parpadea el LED
// --------------------------
void tsk_LEDS(void *params)
{
    uint16_t blocking_time;

    while (1)
    {
        // Lee el último valor de luminosidad
        xQueuePeek(queue_lux, &blocking_time, portMAX_DELAY);
        // Máximo es aprox 30000 entonces 3000 ms como máximo
        blocking_time /= 10;
        // Conmuto salida
        wrapper_output_toggle((gpio_t){LED});
        // Bloqueo el tiempo que se indique de la cola
        vTaskDelay(pdMS_TO_TICKS(blocking_time));
    }
}

// --------------------------
// Tarea 8: Buzzer
// --------------------------
void tsk_buzzer(void *params)
{
    while (1)
    {
        xSemaphoreTake(semphr_buzz, portMAX_DELAY);
        // Buzzer activo
        wrapper_output_toggle((gpio_t){BUZZER});
    }
}

// ------------------------------------------
// Tarea 9: Tarea que decrementa un contador
// ------------------------------------------
void tsk_counter(void *params)
{
    while (1)
    {
        // Decrementa la cuenta cada un segundo
        xSemaphoreTake(semphr_counter, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// ------------------------------------------
// Tarea 10: Tarea que controla el contador
// ------------------------------------------
void tsk_counter_btns(void *params)
{
    while (1)
    {
        // Toma el mutex para bloquear la otra tarea que escribe el display
        xSemaphoreTake(semphr_mutex, portMAX_DELAY);
        // Verifica qué pulsador se presionó
        if (wrapper_btn_get_with_debouncing_with_pull_up((gpio_t){S1_BTN}))
        {
            // Decrementa la cuenta del semáforo
            xSemaphoreTake(semphr_counter, 0);
        }
        else if (wrapper_btn_get_with_debouncing_with_pull_up((gpio_t){S2_BTN}))
        {
            // Incrementa la cuenta del semáforo
            xSemaphoreGive(semphr_counter);
        }
        // Escribe en el display
        uint16_t data = uxSemaphoreGetCount(semphr_counter);
        xQueueOverwrite(queue_display, &data);
        // Demora chica para evitar que detecte muy rápido que se presionó
        vTaskDelay(pdMS_TO_TICKS(30));
        // Devuelve el mutex
        xSemaphoreGive(semphr_mutex);
    }
}