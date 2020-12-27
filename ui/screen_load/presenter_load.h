#ifndef _presenter_charger_h_
#define _presenter_Charger_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "bui.h"
#include "model_psu.h"
#include "view_load.h"

typedef enum{
    LOAD_INIT = 0,
    LOAD_ENTER_IDLE,
    LOAD_IDLE,
    LOAD_EXIT,
    LOAD_UPD,
    LOAD_ENABLE_LOAD,
    LOAD_RUNNING,
}stateload_e;


class PresenterLoad : public BUIPresenter{
public:
    PresenterLoad() : _state(LOAD_INIT){ _view = NULL;}
    void init(void);
    void tick(void);
    void update(void);
    buievt_e eventHandler(buikeyevt_t *evt);
    void setModel(BUIModel *m){_model = (ModelPsu*)m;}   
    BUIView *getView(void){ return (BUIView*)_view;}
private:
    buievt_e stateIdle(buikeyevt_t *evt);
    buievt_e stateRunning(buikeyevt_t *evt);
    void computeCapacity(uint32_t ma);
    ModelPsu *_model;
    ViewLoad *_view;
    stateload_e _state;
    float _capacity;
    uint32_t _elapsed_ticks;
};

#ifdef __cplusplus
}
#endif

#endif /* _presenter_preset_h_ */