#ifndef _view_load_h_
#define _view_load_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "psu.h"
#include "bui.h"

class ViewLoad : public BUIView{
public:
    ViewLoad();
    ~ViewLoad(){}
    void init(void);
    void draw(void);
private:
    
};

#ifdef __cplusplus
}
#endif

#endif /* _view_preset_h_ */