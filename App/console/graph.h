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
    /**
     * @brief Configure graph
     * 
     * \param x,y : X,Y graph position
     * \param xsize : number of points on X axis
     * \param ysize : number of points on Y axis
     * \param plt : Color palette, plt[0] Graph background
     *                             plt[1] Graph axis color
     *                             plt[2] trace 1 color
     *                             ....
     * */
    void init(uint8_t x, uint8_t y, uint8_t xsize, uint8_t ysize, uint16_t *plt);
    void clear();
    void redraw();
    /**
     * @brief Adds point to graph, each point is added to the last x.
     * When x is equal to graph width the graph starts scrolling and each new
     * point added is ploted on to graph end.
     * 
     * \param value : y value
     * \param flags : bits 7:4 not used, bits 3:0 trace
     * */
    void addPoint(uint8_t value, uint8_t flags);
    void nextPoint();
    void update();
    void reset();
};

extern uint8_t graph_data[GRAPH_MAX_TRACE][GRAPH_MAX_DATA];

#ifdef __cplusplus
}
#endif

#endif // !_adcmux_h_
