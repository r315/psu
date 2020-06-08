#ifndef _graph_h_
#define _graph_h_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GRAPH_MAX_DATA  80
#define GRAPH_MAX_TRACE 2

class Graph {
    uint16_t xsize, ysize, posx, posy, *pal;
    uint8_t scroll;
    uint8_t head, tail; 
    void drawGraphAxis();
public:
    void init(uint8_t x, uint8_t y, uint8_t xsize, uint8_t ysize, uint16_t *plt);
    void clear();
    void redraw();
    void addPoint(uint8_t value, uint8_t trace);
    void nextPoint();
    void update();
};

extern uint8_t graph_data[GRAPH_MAX_TRACE][GRAPH_MAX_DATA];

#ifdef __cplusplus
}
#endif

#endif // !_adcmux_h_
