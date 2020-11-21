#include "bui.h"
#include "lcd.h"
#include "font.h"

static BUI *bui;

void BUI::handler(void *ptr){
    //BUI *pbui = (BUI*)ptr;
    
    // read input

    BUIScreen *scr = this->getActiveScreen();

    switch(BUTTON_Read()){
        case BUTTON_PRESSED:
            scr->keyPressed(BUTTON_GetValue());
            break;

        case BUTTON_RELEASED:            
            scr->keyReleased(BUTTON_GetValue());
            break;

        case BUTTON_HOLD:
            break;

        default:
            break;
    }

    // Check invalidated elements
    struct bui_node *node = scr->getWidgets();

    while(node != NULL){
        BUIWidget *wi = (BUIWidget*)node->elem;
        if(wi != NULL){
            wi->draw();
        }
        node = node->next;            
    }
    // update display    
}

void BUI::init(void){
    _act_scr = NULL;
    _scrlist.elem = NULL;
    _scrlist.next = NULL;
}

/**
 * @brief Add a screen and make it active if there is no
 * active screen
 * 
 * \param screen : poiter to screen to be addded
 * \return : index given to the screen
 * */
uint8_t BUI::addScreen(BUIScreen *screen){
    
    uint8_t idx = buiAddNode(&_scrlist, (void *)screen);
    
    if(_act_scr == NULL){
        setActiveScreen(idx);
    }

    return idx;
}

void BUI::setActiveScreen(uint8_t idx){
    struct bui_node *head = &_scrlist;
    while(idx--){
        head = head->next;
    }
    _act_scr = (BUIScreen*)head->elem;
}

BUIScreen *BUI::getActiveScreen(void){
    return _act_scr;
}

void BUI::nextScreen(void){
    /*if(_act_scr->next != NULL){
        _act_scr = _act_scr->next;
    }else{
        _act_scr = (BUIScreen*)_scrlist.elem;
    } */   
}

uint8_t buiAddNode(struct bui_node *head, void *elem){
    uint8_t idx = 0;

    // First element
    if(head->elem == NULL){
        head->elem = (struct bui_node*)elem;
        return idx;        
    }

    struct bui_node *node = (struct bui_node*)bui_malloc(sizeof(struct bui_node));

    if(node == NULL){
        // Unable to allocate
        return 0;
    }   

    node->elem = (struct bui_node*)elem;
    node->next = NULL;

    while(head->next != NULL){
        head = head->next;
        idx++;
    }

    head->next = node;

    return idx;
}

void buiInit(void){
    static BUI def;
    bui = &def;
    bui->init();
}

void buiAddScreen(BUIScreen *scr){
    bui->addScreen(scr);
}

void setActiveScreen(uint8_t idx){
    bui->setActiveScreen(idx);
}

void bui_handler(void *ptr){
    bui->handler(ptr);
}
