
#include <string.h>
#include "bui.h"

//uint8_t graph_data[GRAPH_MAX_TRACE][GRAPH_MAX_DATA];

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
BUIGraph::BUIGraph(uint8_t x, uint8_t y, uint8_t xsz, uint8_t ysz, uint8_t ntraces, const uint16_t *plt) : BUIWidget(x, y){
    _graph_data = (uint8_t *)bui_malloc((xsz - 1) * ntraces);

    if(_graph_data == NULL){
        return;
    }
    
    _pal = plt;
    _xsize = xsz - 1;
    _ysize = ysz;
    _ntraces = ntraces;

    setFlag(BUI_FLAG_REDRAW);
    reset();
}

BUIGraph::~BUIGraph(){
    if(_graph_data != NULL){
        bui_free(_graph_data);
    }
}
/**
 * @brief Clears graph
 * */
void BUIGraph::clear(){
    DRAW_FillRect(_x, _y, _xsize, _ysize, _pal[0]);
}

void BUIGraph::redraw(){
    setFlag(BUI_FLAG_REDRAW);
}

/**
 * @brief Redraws full graph and plots current graph data
 * */
void BUIGraph::draw(){    
  
    if(isInvalid()){
        uint8_t tt = _tail;
        uint8_t npoints;

        if(isFlagSet(BUI_FLAG_REDRAW)){
            drawGraphAxis();
            clrFlag(BUI_FLAG_REDRAW);
        }

        if(_scroll){
            npoints = _xsize;
        }else{
            npoints = (_head > _tail) ? _head - _tail : _xsize - _tail + _head;
        }

        for(uint8_t i = 0; i < npoints; i++){
            // Erase graph data for current point
            DRAW_VLine(_x + i, _y + 1, _ysize, _pal[0]);

            for(uint8_t trace = 0; trace < _ntraces; trace++){
                uint8_t data = _graph_data[(trace * _xsize) + tt];
                if(data < _ysize){
                    DRAW_Pixel(_x + i, _y + _ysize - data, _pal[trace + 2]);
                }
            }        
            
            if( (++tt) >= _xsize){
                tt = 0;
            }
        }

        // fill remaning graph, if any with background
        for(uint8_t i = npoints; i < _xsize; i++){
            DRAW_VLine(_x + i, _y + 1, _ysize, _pal[0]);
        }

        setInvalid(false);
    }
}

void BUIGraph::drawGraphAxis(void){
    DRAW_VLine(_x - 1, _y, _ysize, _pal[1]);
    DRAW_HLine(_x, _y + _ysize + 1, _xsize, _pal[1]);
}

/**
 * @brief Adds point to graph data buffer, each point is added to the last x.
 * When x is equal to graph width the graph starts scrolling and each new
 * point added is ploted on to graph end.
 * 
 * \param value : y value
 * \param flags : bits 7:4 not used, bits 3:0 trace index
 * */
void BUIGraph::addPoint(uint8_t *value, uint8_t ntrace){
    
    for(uint8_t i = 0; i < ntrace; i++, value++){
        if(*value > _ysize){
            *value = _ysize;
        }
        _graph_data[(i * _xsize) + _head] = *value;
    }
    nextPoint();
    setInvalid(true);
}

void BUIGraph::nextPoint(){
    _head++;

    if(_head == _xsize){
        _scroll = true;
    }

    if(_head == _xsize){
        _head = 0;
    }

    if(_scroll){
        _tail++;
        if(_tail == _xsize){
            _tail = 0;
        }
    }
}

/**
 * @brief Clears and reset graph data buffer
 * */
void BUIGraph::reset(){
    _tail = _head = 0;
    _scroll = false;
    // Set all traces above maximum for not to be drawn
    memset(_graph_data, _ysize + 1, _xsize * _ntraces);
    setInvalid(true);
}