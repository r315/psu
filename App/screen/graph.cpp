
#include "graph.h"
#include "draw.h"
#include "psu.h"

uint8_t graph_data[2][GRAPH_MAX_DATA];

void Graph::init(uint8_t x, uint8_t y, uint8_t xsz, uint8_t ysz, uint16_t *plt){
    pal = plt;
    xsize = xsz - 1;
    ysize = ysz;
    posx = x + 1;
    posy = y;
    scroll = false;
    head = tail = 0;
}

void Graph::clear(){
    DRAW_FillRect(posx, posy, xsize, ysize, pal[0]);
    memset(graph_data, 0 , sizeof(graph_data));
}

void Graph::redraw(){
    drawGraphAxis();
    update();
}

void Graph::drawGraphAxis(void){
    DRAW_VLine(posx - 1, posy, ysize, pal[1]);
    DRAW_HLine(posx, posy + ysize + 1, xsize, pal[1]);
}

void Graph::addPoint(uint8_t value, uint8_t flags){
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
        npoints = (head > tail)? head - tail : xsize - tail + head;
    }

    for(uint8_t i = 0; i < npoints; i++){
        DRAW_VLine(posx + i, posy + 1, ysize, pal[0]);

        for(uint8_t trace = 0; trace < GRAPH_MAX_TRACE; trace++){
            uint8_t data = graph_data[trace][tt];            
            DRAW_Pixel(posx + i, posy + ysize - data, pal[trace + 2]);
        }        
        
        if( (++tt) > GRAPH_MAX_DATA){
            tt = 0;
        }
    }
}

void Graph::reset(){
    clear();
    tail = head = 0;
    scroll = false;
}