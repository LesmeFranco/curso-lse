#include "board.h"
#include "pin_mux.h"
#include "fsl_dac.h"
#include "fsl_power.h"
#include "fsl_swm.h"
#include "fsl_iocon.h"
#include "fsl_clock.h"

int main(void)
{
    // Configurar la salida del DAC al PO 29
    CLOCK_EnableClock(kCLOCK_Swm);
    SWM_SetFixedPinSelect(SWM0, kSWM_DAC_OUT1, true);
    CLOCK_DisableClock(kCLOCK_Swm);

    // Habilito la función del DAC en el PO 29
    CLOCK_EnableClock(kCLOCK_Iocon);
    IOCON_PinMuxSet(IOCON, 1, IOCON_PIO_DACMODE_MASK);
    CLOCK_DisableClock(kCLOCK_Iocon);

    // Prendo el DAC
    POWER_DisablePD(kPDRUNCFG_PD_DAC1);

    // Configuro el DAC con 1us de refresco
    dac_config_t dacConfig = {kDAC_SettlingTimeIs1us};
    DAC_Init(DAC1, &dacConfig);

    // Offset 1.65V
    DAC_SetBufferValue(DAC1, 512);

    // Configuro SysTick para 62.5us
    SysTick_Config(SystemCoreClock / 16000);

    while (1)
        ;
    return 0;
}

// Tabla 16 Valores de la señal senoidal
const uint32_t values[16] = {
    0, 64, 128, 191, 255, 319, 383, 447,
    511, 575, 639, 703, 767, 831, 895, 959};

// Variable volatil para cambiar generar valores de señal senoidal
volatile uint8_t Wave = 0;

void SysTick_Handler(void)
{
    DAC_SetBufferValue(DAC1, values[Wave]);
    Wave++;
    if (Wave >= 16)
    {
        Wave = 0; // Reinicio la senoidal
    }
}