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
    void destroy(void);
    void update(void);
    uint8_t eventHandler(buievt_t *evt);
    void setModel(BUIModel *m){_model = (ModelPsu*)m;}
    BUIView *getView(void){ return (BUIView*)_view;}
private:
    ViewPsu *_view;
    ModelPsu *_model;
    statepsu_e _state;
    void stateEnabled(buievt_t *evt);
    void stateIdle(buievt_t *evt);
    void stateSetV(buievt_t *evt);
    void stateSetI(buievt_t *evt);
};

#ifdef __cplusplus
}
#endif

#endif /* _presenter_psu_h_ */