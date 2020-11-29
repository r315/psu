#ifndef _presenter_preset_h_
#define _presenter_preset_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "bui.h"
#include "view_preset.h"
#include "model_psu.h"


class ViewPreset;

class PresenterPreset : public BUIPresenter{
public:
    PresenterPreset(ViewPreset &v) : _view(v){}
    void update(void);
    void eventHandler(buievt_t *evt);
    void setModel(BUIModel *m){_model = (ModelPsu*)m;}
    BUIView &getView(void){ return (BUIView&)_view;}
private:
    ViewPreset &_view;
    ModelPsu *_model;
};

#ifdef __cplusplus
}
#endif

#endif /* _presenter_preset_h_ */