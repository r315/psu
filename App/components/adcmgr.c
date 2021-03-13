#include "adcmgr.h"
#include "board.h"
#include "psu.h"


typedef void (*mgr_cb_t)(uint16_t*);

static uint16_t adcmgr_ch_data[AN_MUX_NUM_CH];
static mgr_cb_t mgr_cb;
static uint8_t *seq;
static uint8_t seq_len, seq_idx;
static const uint8_t adcmgr_seq[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

#define ADCMGR_DEFAULT_SEQ      adcmgr_seq

// https://www.electronicdesign.com/technologies/analog/article/21778422/use-software-filters-to-reduce-adc-noise
//#define MRG_ENABLE_FIR  // Finite impulse response
#define MRG_ENABLE_IIR   // Infinite impulse response

//#define MGR_DEBUG_ENABLE

#if defined(MGR_DEBUG_ENABLE)
static uint16_t raw_data[AN_MUX_NUM_CH];
#endif

#ifdef MRG_ENABLE_FIR
#define MRG_FIR_ORDER  8
static uint16_t fir_acc[AN_MUX_NUM_CH][MRG_FIR_ORDER];
static uint8_t fir_idx = 0;
#endif

#ifdef MRG_ENABLE_IIR
#define MRG_IIR_COUNTS  10
#endif

/**
 * @brief Callback for single channel convertion 
 * in sequence.
 * 
 * \param data : raw adc value 
 * */
static void eoc_seq_cb(uint16_t data){
    //DBG_PIN_TOGGLE;   
    if(!seq){
        return;
    }    
    
    if(seq_idx == seq_len){
        // End here while a new ADCMGR_Start() isn't issued
        if(mgr_cb){
            mgr_cb(adcmgr_ch_data);            
        }
    }else{
#if defined(MGR_DEBUG_ENABLE)
        raw_data[seq_idx] = data;
#endif
#ifdef MRG_ENABLE_FIR
        fir_acc[seq_idx][fir_idx] = data;
        
        uint16_t acc = 0;

        for(uint8_t n = fir_idx; n < fir_idx + MRG_FIR_ORDER; n++){
            acc += fir_acc[seq_idx][ n % MRG_FIR_ORDER];
        }

        adcmgr_ch_data[seq_idx] = acc/MRG_FIR_ORDER;
        
        if(seq_idx == (seq_len - 1)){
            fir_idx++;
            fir_idx = fir_idx % MRG_FIR_ORDER;
        }
#elif defined(MRG_ENABLE_IIR)
        adcmgr_ch_data[seq_idx] += ((data - adcmgr_ch_data[seq_idx])/MRG_IIR_COUNTS);
#else
        adcmgr_ch_data[seq_idx] = data;    
#endif
#if defined(MGR_DEBUG_ENABLE)
        if(seq_idx == 0){
            DBG_PRINT("%d,%d\r", raw_data[seq_idx], adcmgr_ch_data[seq_idx]);
        }
#endif
        ADCMGR_SetChannel(seq[++seq_idx]);
    }
}


/**
 * @brief Callback for single channel convertion
 * 
 * \param data : raw adc value 
 * */
static void eoc_single_cb(uint16_t data){
    adcmgr_ch_data[seq_idx++] = data;
    ADC_Stop();
}


/**
 * @brief Initializes ADC manager 
 * 
 * */
void ADCMGR_Init(void){
    ADC_Init(ADC_SAMPLE_TIME/2);
}

/**
 * @brief Convert single channel
 * 
 * \param channel : Channel to be converted
 * \return : raw adc value
 * */
uint16_t ADCMGR_Convert(uint8_t channel){
    uint16_t res;
    ADC_Stop();
    ADC_SetCallBack(eoc_single_cb);
    ADCMGR_SetChannel(channel);
    seq_idx = 0;
    while(seq_idx == 0){

    }
    res = adcmgr_ch_data[0];
    seq_idx = 0;
    ADC_SetCallBack(eoc_seq_cb);
    ADC_Start();
    return res;
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
    
    mgr_cb = cb;

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
    ADCMGR_SetChannel(seq[seq_idx]);
}

/**
 * @brief Select mux channel
 * \param channel : channel to be selected
 * */
void ADCMGR_SetChannel(uint8_t channel){
    MUX_SELECT(channel & 0x0f);
}