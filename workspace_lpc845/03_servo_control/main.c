#include <stdint.h>
#include <stdio.h>
#include "board.h"
#include "fsl_sctimer.h"
#include "fsl_swm.h"
#include "pin_mux.h"
#include "fsl_gpio.h"
#include "fsl_common.h"
#include "fsl_swm.h"
#include "fsl_debug_console.h"

#define servo_min_pulse_width 500   // Minimo ancho de pulso en microsegundos
#define servo_max_pulse_width 1500  // Maximo ancho de pulso en microsegundos
#define servo_frecuency 50          // Frequencia en Hz (50Hz para servos)

int main(void) {

    //Habilito Clock de matriz de conmutación (SWM)
    CLOCK_EnableClock(kCLOCK_Swm);
    SWM_SetMovablePinSelect(SWM0, kSWM_SCT_OUT0, kSWM_PortPin_P0_18);
    CLOCK_DisableClock(kCLOCK_Swm);

    // Eligo el clock 
    uint32_t sctimer_clock = CLOCK_GetFreq(kCLOCK_fro);
    
    // Configuro el SCT Timer
    sctimer_config_t sctimer_config;
    SCTIMER_GetDefaultConfig(&sctimer_config);
    SCTIMER_Init(SCT0, &sctimer_config);

    //Configuracion de PWM
    sctimer_pwm_signal_param_t pwm_config = {
        .output = kSCTIMER_Out_0,
        .level = kSCTIMER_HighTrue,
        .dutyCyclePercent =  0
    };

    // Evento para el PWM
    uint32_t pwm_event = 0;
    SCTIMER_SetupPwm(SCT0, &pwm_config, servo_frecuency, sctimer_clock, &pwm_event);

    // Inicio el SCTIMER
    SCTIMER_StartTimer(SCT0, kSCTIMER_Counter_U);

    // Configuro el ADC por defecto
    adc_config_t adc_config;
    ADC_GetDefaultConfig(&adc_config);
    ADC_Init(ADC0, &adc_config);

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
		.voltageRange = kADC_HighVoltageRange
	};
    // Habilito el ADC
	ADC_Init(ADC0, &adc_config);

	// Configuracion para las conversiones
	adc_conv_seq_config_t adc_sequence = {
		.channelMask = 1 << ADC_POT_CH,							// Elijo el canal del potenciometro
		.triggerMask = 0,										// No hay trigger por hardware
		.triggerPolarity = kADC_TriggerPolarityPositiveEdge,	// Flanco ascendente
		.enableSyncBypass = false,								// Sin bypass de trigger
		.interruptMode = kADC_InterruptForEachSequence			// Interrupciones para cada secuencia
	};
	
    // Configuro y habilito secuencia A
	ADC_SetConvSeqAConfig(ADC0, &adc_sequence);
	ADC_EnableConvSeqA(ADC0, true);

	// Habilito interrupcion para el ADC y en el NVIC
    ADC_EnableInterrupts(ADC0, kADC_ConvSeqAInterruptEnable);
    NVIC_EnableIRQ(ADC0_SEQA_IRQn);

    // Inicio la conversion del ADC
    ADC_DoSoftwareTriggerConvSeqA(ADC0);

    while(1){
        // Leer el ADC
        adc_result_info_t adc_result;
        ADC_GetChannelConversionValue(ADC0, 0, &adc_result);

        // Calculo el ancho de pulso con el valor del ADC


        // Duty Cycle


        // Actualizo el PWM
        SCTIMER_UpdatePwmDutycycle(SCT0, kSCTIMER_Out_0, duty, kSCTIMER_EdgeAligned, 0);
    }
}