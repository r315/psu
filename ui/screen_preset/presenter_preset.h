#ifndef _presenter_preset_h_
#define _presenter_preset_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "bui.h"
#include "view_preset.h"
#include "model_psu.h"

typedef enum{
    INIT = 0,
    ENTER_IDLE,
    IDLE,
    EXIT,
    UPD,
}statepreset_e;


class ViewPreset;

class PresenterPreset : public BUIPresenter{
public:
    PresenterPreset(ViewPreset &v) : _view(v),_state(INIT){}
    void update(void);
    void eventHandler(buievt_t *evt);
    void setModel(BUIModel *m){_model = (ModelPsu*)m;}
    BUIView &getView(void){ return (BUIView&)_view;}
private:
    void moveSelect(int8_t dir);
    ViewPreset &_view;
    ModelPsu *_model;
    statepreset_e _state;
    uint8_t _idx;
};

#ifdef __cplusplus
}
#endif

#endif /* _presenter_preset_h_ */