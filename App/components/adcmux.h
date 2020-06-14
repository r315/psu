#ifndef _adcmux_h_
#define _adcmux_h_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ADCMUX_MAX_CHANNELS     16

/**
 * @brief
 * */
void ADCMUX_Init();

/**
 * @brief Start channel sequence convertion
 * @param seq : array with channel sequence convertion
 * @param len : sequence length
 * @param cb : callback for end of convertion of all channels on sequence
 * */
void ADCMUX_SetSequence(uint8_t *seq, uint8_t len, void(*cb)(uint16_t *data));

/**
 * @brief Start configured sequence convertion
 * */
void ADCMUX_Start(void);

/**
 * @brief Stop a sequence convertion
 * */
void ADCMUX_StopSequence(void);

/**
 * @brief Convert single channel. Blocking call
 * @param channel : channel to be converted
 * */
uint16_t ADCMUX_Convert(uint8_t channel);

#ifdef __cplusplus
}
#endif

#endif // !_adcmux_h_
