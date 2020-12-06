#ifndef _presenter_psu_h_
#define _presenter_psu_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "bui.h"
#include "view_psu.h"
#include "model_psu.h"

typedef enum{
    PSU_INIT = 0,
    PSU_ENTER_IDLE,
    PSU_IDLE,
    PSU_ENABLED,
    PSU_SET_V,
    PSU_SET_I
}statepsu_e;

class ViewPsu;

class PresenterPsu : public BUIPresenter{
public:
    PresenterPsu() : _view(NULL){ _state = PSU_INIT;}
    void init(void);
    void tick(void);
    void update(void);
    buievt_e eventHandler(buikeyevt_t *evt);
    void setModel(BUIModel *m){_model = (ModelPsu*)m;}
    BUIView *getView(void){ return (BUIView*)_view;}
private:
    ViewPsu *_view;
    ModelPsu *_model;
    statepsu_e _state;
    void stateEnabled(buikeyevt_t *evt);
    void stateIdle(buikeyevt_t *evt);
    void stateSetV(buikeyevt_t *evt);
    void stateSetI(buikeyevt_t *evt);
};

#ifdef __cplusplus
}
#endif

#endif /* _presenter_psu_h_ */