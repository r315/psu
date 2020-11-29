#ifndef _view_charger_h_
#define _view_charger_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "psu.h"
#include "bui.h"

class ViewCharger : public BUIView{
public:
    ViewCharger();
    ~ViewCharger(){}
    void init(void);
    void draw(void);
private:
    
};

#ifdef __cplusplus
}
#endif

#endif /* _view_preset_h_ */