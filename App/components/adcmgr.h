#ifndef _ADCMGR_h_
#define _ADCMGR_h_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ADCMGR_NUM_CHANNELS     16

/**
 * @brief
 * */
void ADCMGR_Init();

/**
 * @brief Start channel sequence convertion
 * @param seq : array with channel sequence convertion
 * @param len : sequence length
 * @param cb : callback for end of convertion of all channels on sequence
 * */
void ADCMGR_SetSequence(uint8_t *seq, uint8_t len, void(*cb)(uint16_t *data));

/**
 * @brief Start configured sequence convertion
 * */
void ADCMGR_Start(void);

/**
 * @brief Stop a sequence convertion
 * */
void ADCMGR_StopSequence(void);

/**
 * @brief Convert single channel. Blocking call
 * @param channel : channel to be converted
 * */
uint16_t ADCMGR_Convert(uint8_t channel);

/**
 * @brief Get channel voltage
 * 
 * \param channel : Channel 
 * \return : voltage in mv
 * */
float ADCMGR_GetChannelVoltage(uint8_t channel);

#ifdef __cplusplus
}
#endif

#endif // !_ADCMGR_h_
