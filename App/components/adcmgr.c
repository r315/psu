#include "adcmgr.h"
#include "board.h"
#include "psu.h"

static uint16_t adcmgr_ch_data[AN_MUX_NUM_CH];
static void (*adcmgr_eoc_cb)(uint16_t *seq);
static uint8_t *seq;
static uint8_t seq_len, seq_idx;

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
 * @brief Callback for single channel convertion
 * 
 * \param data : raw adc value 
 * */
static void eoc_single_cb(uint16_t data){
    adcmgr_ch_data[seq_idx++] = data;
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
    ADCMGR_SetChannel(channel);
    seq_idx = 0;
    ADC_SetCallBack(eoc_single_cb);
    ADC_Start();
    while(seq_idx == 0);
    return adcmgr_ch_data[0];
}

/**
 * @brief Configures a sequence of channels to be converted and End Of Convertion
 * callback. If sequence is already set and converting the caller should wait for
 * the eoc before setting the new sequence
 * ADCMRG_SetSequence.
 * 
 * \param s   : Pointer to channel sequence
 * \param len : Size of sequence
 * \param cb  : Callback for when all channels are converted
 * */
void ADCMGR_SetSequence(uint8_t *s, uint8_t len, void(*cb)(uint16_t *data)){

    if(seq == NULL || len == 0){
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
    if(seq_len == 0){
        return;
    }
    seq_idx = 0;
    ADCMGR_SetChannel(seq_idx);
    ADC_Start();
}

/**
 * @brief Select mux channel
 * \param channel : channel to be selected
 * */
void ADCMGR_SetChannel(uint8_t channel){
    MUX_SELECT(channel);
}