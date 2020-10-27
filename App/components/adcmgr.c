#include "adcmgr.h"
#include "board.h"
#include "psu.h"

static uint16_t adcmgr_ch_data[ADCMGR_NUM_CHANNELS];
static void (*adcmgr_eoc_cb)(uint16_t *seq);
static uint8_t *seq;
static uint8_t seq_len, seq_idx;

// gain = 1/(R2/(R1+R2))
static float adcmgr_ch_gain[ADCMGR_NUM_CHANNELS] = {
    6.08f,  //V_1
    1.0f,  //I_1
    6.1f,  //V_2
    1.0f,  //I_2
    6.1f,  //V_3
    0.0f,
    0.0f,
    0.0f,
    1.0f,  //VB+
    1.0f,  //VB3
    1.0f,  //VB2
    1.0f,  //VB1
    1.0f,  //VB-
    1.0f,  //I_USB
    0.0f,
    0.0f
};

static const uint8_t adcmgr_seq[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

#define ADCMGR_DEFAULT_SEQ      adcmgr_seq
#define ADCMGR_DEFAULT_GAIN     adcmgr_ch_gain

/**
 * @brief Callback for single channel convertion 
 * in sequence.
 * 
 * \param data : raw adc value 
 * */
static void eoc_seq_cb(uint16_t data){
    adcmgr_ch_data[seq_idx++] = data;
    if(seq_idx == seq_len){
        if(adcmgr_eoc_cb)
            adcmgr_eoc_cb(adcmgr_ch_data);
        return;
    }
    if(seq){
        ADCMGR_SetChannel(seq[seq_idx]);
        ADC_Start();
    }
}

/**
 * @brief Initializes ADC manager 
 * Each channel is initialized with a default gain
 * */
void ADCMGR_Init(void){
    ADC_Init(0);
    /* for (uint8_t i = 0; i < ADCMGR_NUM_CHANNELS; i++){
       ADCMGR_DEFAULT_GAIN[i] = 1.0f;
    }     */
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
 * callback. If sequence is already set ADCMGR_Stop must be called prior calling
 * ADCMRG_SetSequence.
 * 
 * \param s   : Pointer to channel sequence
 * \param len : Size of sequence
 * \param cb  : Callback for when all channels are converted
 * */
void ADCMGR_SetSequence(uint8_t *s, uint8_t len, void(*cb)(uint16_t *data)){

    if(seq == NULL){
        seq = (uint8_t*)ADCMGR_DEFAULT_SEQ;
        seq_len = sizeof(ADCMGR_DEFAULT_SEQ);
    }else{
        seq = s;
        seq_len = len;
    }

    seq_idx = 0;
    
    adcmgr_eoc_cb = cb;
    ADC_SetCallBack(eoc_seq_cb);
}

/**
 * @brief Start sequence convertion
 * */
void ADCMGR_Start(void){
    if(adcmgr_eoc_cb == NULL || seq_len == 0){
        return;
    }
    seq_idx = 0;
    ADCMGR_SetChannel(seq_idx);
    ADC_Start();
}

/**
 * @brief Stops sequence conversion
 * */
void ADCMGR_Stop(void){
    if(adcmgr_eoc_cb != NULL){
        while(seq_idx != seq_len){
            seq_len = 0;
            adcmgr_eoc_cb = NULL;
        }
    }
}

/**
 * @brief Get channel voltage
 * 
 * \param channel : Channel 
 * \return : voltage in mv
 * */
float ADCMGR_GetChannelVoltage(uint8_t channel){
    return adcmgr_ch_data[channel] * ADC_GetResolution() * ADCMGR_DEFAULT_GAIN[channel];
}

/**
 * @brief Select mux channel
 * \param channel : channel to be selected
 * */
void ADCMGR_SetChannel(uint8_t channel){
    MUX_SELECT(channel);
}