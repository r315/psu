
#include "graph.h"
#include "draw.h"
#include "psu.h"

uint8_t graph_data[GRAPH_MAX_TRACE][GRAPH_MAX_DATA];

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
void Graph::init(uint8_t x, uint8_t y, uint8_t xsz, uint8_t ysz, uint16_t *plt){
    pal = plt;
    xsize = xsz - 1;
    ysize = ysz;
    posx = x + 1;
    posy = y;
    reset();
}

/**
 * @brief Clears graph
 * */
void Graph::clear(){
    DRAW_FillRect(posx, posy, xsize, ysize, pal[0]);
}

/**
 * @brief Redraws full graph and plots current graph data
 * */
void Graph::redraw(){
    drawGraphAxis();
    update();
}

void Graph::drawGraphAxis(void){
    DRAW_VLine(posx - 1, posy, ysize, pal[1]);
    DRAW_HLine(posx, posy + ysize + 1, xsize, pal[1]);
}

/**
 * @brief Adds point to graph data buffer, each point is added to the last x.
 * When x is equal to graph width the graph starts scrolling and each new
 * point added is ploted on to graph end.
 * 
 * \param value : y value
 * \param flags : bits 7:4 not used, bits 3:0 trace
 * */
void Graph::addPoint(uint8_t value, uint8_t flags){
    if(value > ysize){
        value = ysize;
    }
    graph_data[flags & 0x0F][head] = value;
}

void Graph::nextPoint(){
    head++;

    if(head == xsize){
        scroll = true;
    }

    if(head == GRAPH_MAX_DATA){
        head = 0;
    }

    if(scroll){
        tail++;
        if(tail == GRAPH_MAX_DATA){
            tail = 0;
        }
    }
}

void Graph::update(){
    
    uint8_t tt = tail;

    uint8_t npoints;

    if(scroll){
        npoints = xsize;
    }else{
        npoints = (head > tail) ? head - tail : xsize - tail + head;
    }

    for(uint8_t i = 0; i < npoints; i++){
        // Erase graph data for current point
        DRAW_VLine(posx + i, posy + 1, ysize, pal[0]);

        for(uint8_t trace = 0; trace < GRAPH_MAX_TRACE; trace++){
            uint8_t data = graph_data[trace][tt];
            DRAW_Pixel(posx + i, posy + ysize - data, pal[trace + 2]);
        }        
        
        if( (++tt) >= GRAPH_MAX_DATA){
            tt = 0;
        }
    }
}

/**
 * @brief Clears and reset graph data buffer
 * */
void Graph::reset(){
    tail = head = 0;
    scroll = false;
    memset(graph_data, 0 , sizeof(graph_data));
}