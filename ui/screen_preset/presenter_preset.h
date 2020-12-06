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
    PRE_INIT = 0,
    PRE_ENTER_IDLE,
    PRE_IDLE,
    PRE_EXIT,
    PRE_CHG,
    PRE_SET,
}statepreset_e;


class ViewPreset;

class PresenterPreset : public BUIPresenter{
public:
    PresenterPreset() : _view(NULL),_state(PRE_INIT){}
    void init(void);
    void tick(void);
    void update(void);
    buievt_e eventHandler(buikeyevt_t *evt);
    void setModel(BUIModel *m){_model = (ModelPsu*)m;}
    BUIView *getView(void){ return (BUIView*)_view;}
private:
    void moveSelect(int8_t dir);
    ViewPreset *_view;
    ModelPsu *_model;
    statepreset_e _state;
    uint8_t _idx;
};

#ifdef __cplusplus
}
#endif

#endif /* _presenter_preset_h_ */