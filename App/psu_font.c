
#include <font.h>
#if 0
const uint8_t font_seven_seg_data[] = {
0x7f,0xe0,0xff,0xf0,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xff,0xf0,0x7f,0xe0,
0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,
0xff,0xe0,0xff,0xf0,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0xff,0xf0,0xff,0xf0,0xc0,0x00,0xc0,0x00,0xc0,0x00,0xc0,0x00,0xc0,0x00,0xc0,0x00,0xc0,0x00,0xff,0xf0,0x7f,0xf0,
0xff,0xe0,0xff,0xf0,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0xff,0xf0,0xff,0xf0,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0xff,0xf0,0xff,0xe0,
0x00,0x00,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xff,0xf0,0xff,0xf0,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,
0x7f,0xf0,0xff,0xf0,0xc0,0x00,0xc0,0x00,0xc0,0x00,0xc0,0x00,0xc0,0x00,0xc0,0x00,0xc0,0x00,0xff,0xf0,0xff,0xf0,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0xff,0xf0,0xff,0xe0,
0x7f,0xf0,0xff,0xf0,0xc0,0x00,0xc0,0x00,0xc0,0x00,0xc0,0x00,0xc0,0x00,0xc0,0x00,0xc0,0x00,0xff,0xe0,0xff,0xf0,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xff,0xf0,0x7f,0xe0,
0xff,0xe0,0xff,0xf0,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,
0x3f,0xe0,0xff,0xf0,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xff,0xf0,0xff,0xf0,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xff,0xf0,0x7f,0xe0,
0x7f,0xe0,0xff,0xf0,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xff,0xf0,0xff,0xf0,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x7f,0xf0,0x7f,0xe0,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

font_t font_seven_seg = {
    .w = 12,
    .h = 20,
    .data = font_seven_seg_data,
    .data_len = sizeof(font_seven_seg_data),
    .bpl = 2,
    .offset = '0',
    .spacing = 1
};
#endif
/**
 * @brief Application icons data.
 * data is left aligned
 * */
const uint8_t icon_psu[] = {15,8,
    0x7f,0xfc,0xc4,0x56,0xd5,0xd6,0xd6,0xd6,0xc7,0x56,0xdf,0x56,0xdc,0x46,0x7f,0xfc
};

const uint8_t icon_load[] = {17,8,
    0x7f,0xff,0x00,0xd1,0x1d,0x80,0xd5,0x51,0x80,0xd5,0xd5,0x80,0xd5,0x15,0x80,0xd5,0x55,0x80,0xd1,0x11,0x80,0x7f,0xff,0x00
};

const uint8_t icon_chr[] = {15,8,
    0x7f, 0xfc,0xc5,0x46,0xdd,0x56,0xdd,0x56,0xdc,0x4e,0xdd,0x56,0xc5,0x56,0x7f,0xfc
};

const uint8_t dro_unit_v[] = {12,20,
0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x19,0x80,0x19,0x80,0x19,0x80,0x0f,0x00,0x0f,0x00,0x00,0x00,0x00,0x00,
};

const uint8_t dro_unit_a[] = {12,20,
0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x00,0x1f,0x80,0x30,0xc0,0x30,0xc0,0x30,0xc0,0x60,0x60,0x60,0x60,0x60,0x60,0x7f,0xe0,0x7f,0xe0,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x60,0x00,0x00,0x00,0x00,
};