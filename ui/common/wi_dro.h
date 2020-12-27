#ifndef _wi_dro_h_
#define _wi_dro_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "bui.h"

class BUIdro : public BUIText{
public:
    BUIdro();
    BUIdro(uint16_t x, uint16_t y);
    ~BUIdro(){}
    void init(uint32_t base_pow, uint32_t init_pow, int32_t min, int32_t max, void (*)(char *, int32_t));
    void draw(void);
    void setValue(int32_t newvalue);
    int32_t getValue(void){return _value;}
    void editValue(int8_t dig);
    void changeValue(int8_t dt);
private:
    void update(void);
    void (*_formater)(char *out, int32_t value);
    uint32_t _pow, _min_pow, _max_pow;
    int32_t _value, _max_value, _min_value;
    uint8_t _count;
};

void currentFormat(char *out, int32_t ma);
void voltageFormat(char *out, int32_t mv);

#ifdef __cplusplus
}
#endif

#endif /* _scr_psu_h_ */