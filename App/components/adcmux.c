#include "adcmux.h"
#include "board.h"

static uint8_t *seq;
static uint16_t adcmux_ch[ADCMUX_MAX_CHANNELS];
static uint8_t seq_len, seq_idx;
static void (*adcmux_eoc_cb)(uint16_t *seq);

static void adc_mux_select_ch(uint8_t channel){
    //TODO:
}

static void adcmux_cb(uint16_t data){
    adcmux_ch[seq_idx] = data;
    if(++seq_idx == seq_len){
        adcmux_eoc_cb(adcmux_ch);
        return;
    }
    adc_mux_select_ch(seq[seq_idx]);
    ADC_Start();
}

/**
 * @brief
 * */
void ADCMUX_Init(void){
    ADC_Init(0);
}

/**
 * @brief
 * */
uint16_t ADCMUX_Convert(uint8_t channel){
    return 0;
}

/**
 * @brief
 * */
void ADCMUX_SetSequence(uint8_t *s, uint8_t len, void(*cb)(uint16_t *data)){
    seq = s;
    seq_idx = 0;
    seq_len = len;
    adcmux_eoc_cb = cb;
    ADC_SetCallBack(adcmux_cb);
}

void ADCMUX_Start(void){
    if(adcmux_eoc_cb == NULL || seq_len == 0){
        return;
    }
    seq_idx = 0;
    ADC_Start();
}
