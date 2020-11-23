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
    PSU_IDLE = 0,
    PSU_ENABLED,
    PSU_ENTER_IDLE,
    PSU_INIT
}statepsu_e;

class PresenterPsu : public BUIPresenter{
private:
    ViewPsu *_view;
    ModelPsu *_model;
    statepsu_e _state;
public:
    PresenterPsu() : _view(0){ _state = PSU_INIT;}
    void notify(void);
    void eventHandler(buievt_t *evt);
    void setModel(BUIModel *m){_model = (ModelPsu*)m;}
    void setView(BUIView *v){_view = (ViewPsu*)v;}
};

#ifdef __cplusplus
}
#endif

#endif /* _presenter_psu_h_ */