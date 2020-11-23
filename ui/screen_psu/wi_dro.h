#ifndef _wi_dro_h_
#define _wi_dro_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "bui.h"
#include "wi_dro.h"

class BUIdro : public BUIText{
public:
    BUIdro();
    BUIdro(uint16_t x, uint16_t y);
    ~BUIdro(){}
    void init(uint32_t base_pow, uint32_t init_pow, int32_t min, int32_t max, void (*)(char *, int32_t));
    void edit(int8_t dig);
    void changeDigit(int8_t dt);
    void draw(void);
    void setValue(int32_t newvalue);
    int32_t getValue(void){return _value;}
private:
    void update(void);
    void (*_formater)(char *out, int32_t value);
    uint32_t _pow, _min_pow, _max_pow;
    int32_t _value, _max_value, _min_value;
    uint8_t _count;
};

#ifdef __cplusplus
}
#endif

#endif /* _scr_psu_h_ */