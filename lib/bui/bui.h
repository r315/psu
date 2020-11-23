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

#define BUI_FLAG_INVALID            (1 << 1)
#define BUI_FLAG_VISIBLE            (1 << 0)
#define BUI_FLAG_SUSPEND            (1 << 2)
#define BUI_FLAG_EDIT           (1 << 3)

struct list_node{
    void *elem;
    struct list_node *next;
};

typedef struct buievt{
    uint8_t key;
    uint8_t type;
}buievt_t;


class BUIWidget{
private:
    uint8_t _flags;
protected:
    uint16_t _x, _y;
    void setFlag(uint8_t flag){this->_flags |= flag;}
    void clrFlag(uint8_t flag){this->_flags &= ~flag;}
public:
    BUIWidget(uint16_t x, uint16_t y){ _x = x; _y = y; _flags = BUI_FLAG_VISIBLE;}
    uint8_t isVisible(){ return !!(this->_flags & BUI_FLAG_VISIBLE); }
    uint8_t isInvalid(){ return !!(this->_flags & BUI_FLAG_INVALID); }
    uint8_t isFlagSet(uint8_t flag){return !!(this->_flags & flag);}
    void setInvalid(uint8_t inv){ if(inv) setFlag(BUI_FLAG_INVALID); else clrFlag(BUI_FLAG_INVALID);}
    virtual void draw(void){}
};

class BUIText : public BUIWidget {
public:
    BUIText() : BUIText(0,0){}
    BUIText(uint16_t x, uint16_t y);
    ~BUIText();
    virtual void draw(void);
    void setText(const char *text);
    void setPal(const uint16_t *pal);
    void setFont(font_t *font);
protected:
    font_t *_font;
    const uint16_t *_pal;
    char *_text;
    uint8_t _len;
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
    void addPoint(uint8_t *value, uint8_t ntrace);
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

uint16_t listInsert(struct list_node *head, void *elem);

class BUIView{
public:
    BUIView(){ 
        _widget.elem = NULL;  
        _widget.next = NULL;
    }
    virtual void init(){}    
    uint8_t addWidget(BUIWidget *wi){ return listInsert(&_widget, (void *)wi);}
    list_node *getWidgets(void){ return &_widget; }
    uint8_t isVisible(){ return !!(this->_flags & BUI_FLAG_VISIBLE); }
    uint8_t isInvalid(){ return !!(this->_flags & BUI_FLAG_INVALID); }
    uint8_t isSuspending(){ return !!(this->_flags & BUI_FLAG_SUSPEND); }

    void setFlag(uint8_t flag){this->_flags |= flag;}
    void clrFlag(uint8_t flag){this->_flags &= ~flag;}

private:    
    struct list_node _widget;
    uint8_t _flags;
protected:
};

class BUIModel; 

class BUIPresenter{
public:
    virtual void notify(void);
    virtual void eventHandler(buievt_t *evt);
    virtual void setModel(BUIModel *m);
    virtual void setView(BUIView *v);
};

class BUIModel{
public:
    void setPresenter(BUIPresenter *pre){ _presenter = pre;}
protected:
    BUIPresenter *_presenter;
};

typedef struct bui_screen{
    BUIView *view;
    BUIPresenter *presenter;
}buiscreen_t;

class BUI{    
public:
    BUI(BUIModel &m);
    uint8_t createScreen(BUIView *view, BUIPresenter *pre);
    void setActiveScreen(uint8_t idx);
    void ActivateNextScreen(void);
    void handler(void *ptr);
protected:
private:
    struct list_node _scrlist;
    buiscreen_t *_screen;
    BUIModel &_model;
};

#ifdef __cplusplus
}
#endif


#endif /* _bui_h_ */