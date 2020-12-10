#ifndef _presenter_charger_h_
#define _presenter_Charger_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "bui.h"
#include "model_psu.h"
#include "view_charger.h"

typedef enum{
    CHG_INIT = 0,
    CHG_ENTER_IDLE,
    CHG_IDLE,
    CHG_EXIT,
    CHG_ENTER_CHARGING,
    CHG_CHARGING,
    CHG_SET_I,
    CHG_SET_TYPE,
    CHG_ABORT_SET_TYPE,
    CHG_END_SET_TYPE,
    CHG_ABORT_SET_I,
    CHG_END_SET_I
}statecharger_e;


class PresenterCharger : public BUIPresenter{
public:
    PresenterCharger() : _state(CHG_INIT){ _view = NULL; _ncell = 4; }
    void init(void);
    void tick(void);
    void update(void);
    buievt_e eventHandler(buikeyevt_t *evt);
    void setModel(BUIModel *m){_model = (ModelPsu*)m;}   
    BUIView *getView(void){ return (BUIView*)_view;}
private:
    void stateIdle(buikeyevt_t *evt);
    void stateCharging(buikeyevt_t *evt);
    void stateSetI(buikeyevt_t *evt);
    void stateSetType(buikeyevt_t *evt);
    ViewCharger *_view;
    ModelPsu *_model;
    statecharger_e _state;
    uint8_t _ncell;
};

#ifdef __cplusplus
}
#endif

#endif /* _presenter_preset_h_ */