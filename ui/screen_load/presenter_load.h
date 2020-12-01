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
}stateload_e;


class PresenterLoad : public BUIPresenter{
public:
    PresenterLoad() : _state(LOAD_INIT){ _view = NULL;}
    void init(void);
    void destroy(void);
    void update(void);
    uint8_t eventHandler(buievt_t *evt);
    void setModel(BUIModel *m){_model = (ModelPsu*)m;}   
    BUIView *getView(void){ return (BUIView*)_view;}
private:
    stateload_e _state;    
};

#ifdef __cplusplus
}
#endif

#endif /* _presenter_preset_h_ */