#include "adcmgr.h"
#include "board.h"

static uint16_t adcmgr_ch_data[ADCMGR_MAX_CHANNELS];
static void (*adcmgr_eoc_cb)(uint16_t *seq);
static uint8_t *seq;
static uint8_t seq_len, seq_idx;

static float adcmgr_ch_gain[ADCMGR_MAX_CHANNELS];

/**
 * @brief Select channel on external mux
 * 
 * \param channel : Channel to be selected
 * */
static void adc_mux_select_ch(uint8_t channel){
    //TODO:
}

/**
 * @brief Callback for single channel convertion,
 * This is called from ADC ISR
 * 
 * \param data : raw adc value 
 * */
static void adcmgr_cb(uint16_t data){
    adcmgr_ch_data[seq_idx] = data;
    if(++seq_idx == seq_len){
        adcmgr_eoc_cb(adcmgr_ch_data);
        return;
    }
    adc_mux_select_ch(seq[seq_idx]);
    ADC_Start();
}

/**
 * @brief Initializes ADC manager 
 * Each channel is initialized with a default gain
 * */
void ADCMGR_Init(void){
    ADC_Init(0);
    for (uint8_t i = 0; i < ADCMGR_MAX_CHANNELS; i++)
    {
       adcmgr_ch_gain[i] = 1.0f;
    }
    
}

/**
 * @brief Convert single channel
 * 
 * \param channel : Channel to be converted
 * \return : raw adc value
 * */
uint16_t ADCMGR_Convert(uint8_t channel){
    return 0;
}

/**
 * @brief Configures a sequence of channels to be converted and end of convertion
 * callback
 * 
 * \param s   : Pointer to channel sequence
 * \param len : Size of sequence
 * \param cb  : Callback for when all channels are converted
 * */
void ADCMGR_SetSequence(uint8_t *s, uint8_t len, void(*cb)(uint16_t *data)){
    seq = s;
    seq_idx = 0;
    seq_len = len;
    adcmgr_eoc_cb = cb;
    ADC_SetCallBack(adcmgr_cb);
}

/**
 * @brief Start sequence convertion
 * */
void ADCMGR_Start(void){
    if(adcmgr_eoc_cb == NULL || seq_len == 0){
        return;
    }
    seq_idx = 0;
    ADC_Start();
}

/**
 * @brief Get channel voltage
 * 
 * \param channel : Channel 
 * \return : voltage in mv
 * */
float ADCMGR_GetChannelVoltage(uint8_t channel){
    return adcmgr_ch_data[channel] * ADC_GetResolution() * adcmgr_ch_gain[channel];
}