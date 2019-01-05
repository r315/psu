#ifndef _adc_psuv3_h_
#define _adc_psuv3_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define ADC_TRIGGER_CLOCK 500000UL
#define ADC_CR1_DUALMOD_FAST_INTERLEAVED     (7 << 16)
#define ADC_CR1_DUALMOD_SIMULTANEOUS         (1 << 16)

#define ADC_SAMPLES 4

/* ***********************************************************
 * ADC is triggered by TIM3 TRGO and performs dual 
 * simultaneous convertion. it converts 4 channels and transfers 
 * the result to memory using DMA 
 * 
 * \param ms    Time between events
 * \param dst   pointer to destination, at least 4*uint16_t
 ************************************************************ */
void ADC_Init(uint16_t);

/* ***********************************************************
 * Configure callback for end of transfer of ADC convertions
 * 
 * \param  cb    call back function void cb(uin16_t *adc_convertions);
 * \return none
 ************************************************************ */
void ADC_SetCallBack(void (*)(uint16_t*));

/* ***********************************************************
 * Get the last performed convertions
 * Not thread safe
 * \param  none
 * \return uint16_t *last_adc_convertions 
 ************************************************************ */
uint16_t *ADC_LastConvertion(void);


#ifdef __cplusplus
}
#endif
#endif