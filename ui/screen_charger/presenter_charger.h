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
    CHG_UPD,
}statecharger_e;


class PresenterCharger : public BUIPresenter{
public:
    PresenterCharger(ViewCharger *v) : _view(v), _state(CHG_INIT){}
    void update(void);
    uint8_t eventHandler(buievt_t *evt);
    void setModel(BUIModel *m){_model = (ModelPsu*)m;}   
    BUIView *getView(void){ return (BUIView*)_view;}
private:
    ViewCharger *_view;
    ModelPsu *_model;
    statecharger_e _state;    
};

#ifdef __cplusplus
}
#endif

#endif /* _presenter_preset_h_ */