#include <stdint.h>
#include <stdio.h>
#include "board.h"
#include "fsl_adc.h"
#include "fsl_power.h"
#include "fsl_sctimer.h"
#include "fsl_swm.h"
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "fsl_common.h"
#include "fsl_swm.h"
#include "fsl_debug_console.h"

#define servo_min_pulse_width 500  // Minimo ancho de pulso en microsegundos
#define servo_max_pulse_width 1500 // Maximo ancho de pulso en microsegundos
#define servo_frecuency 50         // Frequencia en Hz (50Hz para servos)
#define ADC_POT_CH 0               // Canal del ADC para el potenciometro

int main(void)
{

    // Habilito Clock de matriz de conmutación (SWM)
    CLOCK_EnableClock(kCLOCK_Swm);
    // Configuro la funcion de ADC en el canal del potenciometro
    SWM_SetFixedPinSelect(SWM0, kSWM_ADC_CHN0, true);
    CLOCK_DisableClock(kCLOCK_Swm);

    // Eligo el clock
    uint32_t sctimer_clock = CLOCK_GetFreq(kCLOCK_Fro);

    // Configuro el SCT Timer
    sctimer_config_t sctimer_config;
    SCTIMER_GetDefaultConfig(&sctimer_config);
    SCTIMER_Init(SCT0, &sctimer_config);

    // Configuracion de PWM
    sctimer_pwm_signal_param_t pwm_config = {
        .output = kSCTIMER_Out_0,
        .level = kSCTIMER_HighTrue,
        .dutyCyclePercent = 0
    };

    // Evento para el PWM
    uint32_t pwm_event;

    // Inicializo el PWM
    SCTIMER_SetupPwm(SCT0, &pwm_config, kSCTIMER_CenterAlignedPwm, servo_frecuency, sctimer_clock, &pwm_event);

    // Inicio el SCTIMER
    SCTIMER_StartTimer(SCT0, kSCTIMER_Counter_U);

    // Prendo el ADC
    POWER_DisablePD(kPDRUNCFG_PD_ADC0);

    // Obtengo frecuencia deseada y calibro ADC
    uint32_t frequency = CLOCK_GetFreq(kCLOCK_Fro) / CLOCK_GetClkDivider(kCLOCK_DivAdcClk);
    ADC_DoSelfCalibration(ADC0, frequency);

    // Configuracion del ADC
    adc_config_t adc_config = {
        .clockMode = kADC_ClockSynchronousMode,
        .clockDividerNumber = 1,
        .enableLowPowerMode = false,
        .voltageRange = kADC_HighVoltageRange};

    // Habilito el ADC
    ADC_Init(ADC0, &adc_config);

    // Configuracion para las conversiones
    adc_conv_seq_config_t adc_sequence = {
        .channelMask = 1 << ADC_POT_CH,                      // Elijo el canal del potenciometro
        .triggerMask = 0,                                    // No hay trigger por hardware
        .triggerPolarity = kADC_TriggerPolarityPositiveEdge, // Flanco ascendente
        .enableSyncBypass = false,                           // Sin bypass de trigger
        .interruptMode = kADC_InterruptForEachSequence       // Interrupciones para cada secuencia
    };

    // Configuro y habilito secuencia A
    ADC_SetConvSeqAConfig(ADC0, &adc_sequence);
    ADC_EnableConvSeqA(ADC0, true);

    // Habilito interrupcion para el ADC y en el NVIC
    ADC_EnableInterrupts(ADC0, kADC_ConvSeqAInterruptEnable);
    NVIC_EnableIRQ(ADC0_SEQA_IRQn);

    // Inicio la conversion del ADC
    ADC_DoSoftwareTriggerConvSeqA(ADC0);

    while (1)
    {
        // Leer el ADC
        adc_result_info_t adc_result;
        ADC_GetChannelConversionResult(ADC0, ADC_POT_CH, &adc_result);
        printf("ADC Result: %ld\n", adc_result.result);

        // Calculo el ancho de pulso con el valor del ADC

        // Duty Cycle
        uint32_t duty_cycle = adc_result.result * 100 / 4095;

        // Actualizo el ancho de pulso
        SCTIMER_UpdatePwmDutycycle(SCT0, kSCTIMER_Out_0, duty_cycle, pwm_event);
    }
}