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

#define BUI_FLAG_VISIBLE            (1 << 0)
#define BUI_FLAG_INVALID            (1 << 1)
#define BUI_FLAG_SUSPEND            (1 << 2)
#define BUI_FLAG_EDIT               (1 << 3)
#define BUI_FLAG_SELECTED           (1 << 4)
#define BUI_FLAG_REDRAW             (1 << 5)

#define BUI_EVT_SEL_SCR(_S)         (buievt_e)(BUI_EVT_SEL_SCR + _S)

struct list_node{
    void *elem;
    struct list_node *next;
};

typedef struct buikeyevt{
    uint8_t key;
    uint8_t type;
}buikeyevt_t;

typedef enum buievt{
    BUI_EVT_NONE = 0,
    BUI_EVT_CHG_SCR,
    BUI_EVT_SEL_SCR,        // This must be the last value
}buievt_e;

class BUIWidget{
public:
    BUIWidget(uint16_t x, uint16_t y){ _x = x; _y = y; _flags = BUI_FLAG_VISIBLE;}
    uint8_t isFlagSet(uint8_t flag){return !!(this->_flags & flag);}
    uint8_t isVisible(){ return isFlagSet(BUI_FLAG_VISIBLE); }
    uint8_t isInvalid(){ return isFlagSet(BUI_FLAG_INVALID); }
    void setInvalid(uint8_t inv){ if(inv) setFlag(BUI_FLAG_INVALID); else clrFlag(BUI_FLAG_INVALID);}
    void setFlag(uint8_t flag){this->_flags |= flag;}
    void clrFlag(uint8_t flag){this->_flags &= ~flag;}
    
    virtual void draw(void){}
    static void invalidateList(list_node *head){
        while(head != NULL){
            ((BUIWidget*)head->elem)->setInvalid(true);
            head = head->next;
        }
    }

    void setPos(uint16_t x, uint16_t y){_x = x; _y = y;}
protected:
    uint16_t _x, _y;
private:
    uint8_t _flags;
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
        _widget_list.elem = NULL;  
        _widget_list.next = NULL;
    }
    virtual ~BUIView(){}
    virtual void init(void){}
    virtual void draw(void){}
    uint8_t addWidget(BUIWidget *wi){ return listInsert(&_widget_list, (void *)wi);}
    list_node *getWidgets(void){ return &_widget_list; }
    uint8_t isFlagSet(uint8_t flag){return !!(this->_flags & flag);}
    uint8_t isInvalid(){ return isFlagSet(BUI_FLAG_INVALID); }
    uint8_t isSuspending(){ return isFlagSet(BUI_FLAG_SUSPEND); }
    void setInvalid(uint8_t inv){ if(inv) setFlag(BUI_FLAG_INVALID); else clrFlag(BUI_FLAG_INVALID);}

    void setFlag(uint8_t flag){this->_flags |= flag;}
    void clrFlag(uint8_t flag){this->_flags &= ~flag;}
    void suspend(void){setFlag(BUI_FLAG_SUSPEND);}
private:    
    struct list_node _widget_list;
    uint8_t _flags;
protected:
};

class BUIModel; 

class BUIPresenter{
public:
    virtual void init(void){}
    virtual void destroy(void){}
    virtual void tick(void);
    virtual void update(void);
    virtual buievt_e eventHandler(buikeyevt_t *evt);
    virtual void setModel(BUIModel *m){_model = m;}
    virtual BUIView *getView(void){ return _view;};
protected:
    BUIView *_view;
    BUIModel *_model;
};

class BUIModel{
public:
    virtual void tick(void){}
    virtual void init(void){}
    void setPresenter(BUIPresenter *pre){ _presenter = pre;}
protected:
    BUIPresenter *_presenter;
};

class BUI{    
public:
    BUI(BUIModel &m);
    uint8_t addPresenter(BUIPresenter *pre);
    void activatePresenterByIdx(uint8_t idx);
    void activateNextPresenter(void);
    void handler(void *ptr);
protected:
private:
    void activatePresenter(BUIPresenter *presenter);
    struct list_node _presenter_list;
    BUIPresenter *_active_presenter;
    BUIModel &_model;
};

#ifdef __cplusplus
}
#endif


#endif /* _bui_h_ */