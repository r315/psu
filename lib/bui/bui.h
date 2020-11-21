/**
 * Bad user interface
 * */
#ifndef _bui_h_
#define _bui_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include "font.h"
#include "strfunc.h"
#include "draw.h"

#include "board.h"
#include "FreeRTOS.h"

#define BUI_DEFAULT_FONT            &courierFont
/* Port macros */
#define bui_malloc                  pvPortMalloc
#define bui_free                    vPortFree
#define bui_createTask              xTaskCreate
#define BUI_HEAP                    512

#define BUI_WI_INVALID              (1 << 1)
#define BUI_WI_VISIBLE              (1 << 0)

#define BUI_WI_SET_FLAG_VISIBLE     this->_flags |= BUI_WI_VISIBLE
#define BUI_WI_CLR_FLAG_VISIBLE     this->_flags &= ~BUI_WI_VISIBLE
#define BUI_WI_SET_FLAG_INVALID     this->_flags |= BUI_WI_INVALID
#define BUI_WI_CLR_FLAG_INVALID     this->_flags &= ~BUI_WI_INVALID

struct bui_node{
    void *elem;
    struct bui_node *next;
};

class BUIWidget{
private:
    uint8_t _flags;
protected:
    uint16_t _x, _y;
public:
    BUIWidget(uint16_t x, uint16_t y){ _x = x; _y = y; _flags = BUI_WI_VISIBLE;}

    void setVisible(uint8_t vi){
        if(vi){
            BUI_WI_SET_FLAG_VISIBLE;
        }else{
            BUI_WI_CLR_FLAG_VISIBLE;
        }
        BUI_WI_SET_FLAG_INVALID;
    }

    void setInvalid(uint8_t inv){ 
        if(inv){
            BUI_WI_SET_FLAG_INVALID;
        }else{
            BUI_WI_CLR_FLAG_INVALID;
        }
    }

    uint8_t isVisible(){ return !!(this->_flags & BUI_WI_VISIBLE); }
    uint8_t isInvalid(){ return !!(this->_flags & BUI_WI_INVALID); }
    virtual void draw(void){}
};

class BUIText : public BUIWidget {
    font_t *_font;
    const uint16_t *_pal;
    char *_text;
    uint8_t _len;

public:
    BUIText() : BUIText(0,0){}
    BUIText(uint16_t x, uint16_t y);
    void draw(void);
    void setText(const char *text);
    void setPal(const uint16_t *pal);
    void setFont(font_t *font);
    ~BUIText();
};

class BUIGraph : public BUIWidget {
public:
    BUIGraph() : BUIWidget(0,0) {}
    ~BUIGraph();
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
    BUIGraph(uint8_t x, uint8_t y, uint8_t xsize, uint8_t ysize, uint8_t ntraces, const uint16_t *plt);
    void clear();
    void draw();
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
private:
    const uint16_t *_pal;
    uint16_t _xsize, _ysize;
    uint8_t _scroll, _ntraces;
    uint8_t _head, _tail;
    uint8_t *_graph_data; //[GRAPH_MAX_TRACE][GRAPH_MAX_DATA];
    void drawGraphAxis();
};
/**
 * */
class BUIScreen{
private:    
    struct bui_node _widget;
protected:
    
public:
    BUIScreen();
    /**
     * Redraws the mode screen with default values
     * called when mode is selected
     * */
    virtual void redraw(){}

    virtual void process(){}

    virtual void init(){}

    virtual void keyPressed(uint32_t key){}
    virtual void keyReleased(uint32_t key){}

    uint8_t addWidget(BUIWidget *wi);
    bui_node *getWidgets(void){ return &_widget; }
};

class BUI{    
public:
    void init(void);
    uint8_t addScreen(BUIScreen *scr);
    BUIScreen *getActiveScreen(void);
    BUIScreen *getScreen(uint8_t idx);
    void setActiveScreen(uint8_t idx);
    void nextScreen(void);
    void handler(void *ptr);
protected:
private:
    struct bui_node _scrlist;
    BUIScreen *_act_scr;
};

void buiInit(void);
void buiAddScreen(BUIScreen *scr);
uint8_t buiAddNode(struct bui_node *list, void *elem);

#ifdef __cplusplus
}
#endif


#endif /* _bui_h_ */