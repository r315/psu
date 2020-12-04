#include "bui.h"
#include "lcd.h"
#include "font.h"


void BUI::handler(void *ptr){   
    
    if(_active_presenter == NULL){
        return;
    }

    BUIPresenter *presenter = _active_presenter;
    BUIView *view = presenter->getView();
    buikeyevt_t evt;

    if(view == NULL || presenter == NULL){
        return;
    }

    // read input
    if(BUTTON_Read() != BUTTON_EMPTY){
        evt.key = BUTTON_GetValue();
        evt.type = BUTTON_GetEvents();
        buievt_e res = presenter->eventHandler(&evt);
        switch(res){
            case BUI_EVT_CHG_SCR:
                presenter->destroy();            
                activateNextPresenter();
                return;

            default:
                if(res >= BUI_EVT_SEL_SCR){
                    uint8_t scr = res - BUI_EVT_SEL_SCR;
                    activatePresenterByIdx(scr);
                }
                break;
        }
    }

    // Draw view elements
    view->draw();

    // Draw invalidated elements
    struct list_node *node = view->getWidgets();

    while(node != NULL){
        BUIWidget *wi = (BUIWidget*)node->elem;
        if(wi != NULL){
            wi->draw();
        }
        node = node->next;            
    }
    // update model
    _model.tick();
}

BUI::BUI(BUIModel &m) : _model(m){
    _active_presenter = NULL;
    _presenter_list.elem = NULL;
    _presenter_list.next = NULL;
}

/**
 * @brief Add a screen and make it active if there is no
 * active screen
 * 
 * \param screen : poiter to screen to be addded
 * \return : index given to the screen
 * */
uint8_t BUI::addPresenter(BUIPresenter *presenter){

    if(presenter == NULL){
        return -1;
    }

    presenter->setModel(&_model);
    
    uint8_t idx = listInsert(&_presenter_list, (void *)presenter);
    
    if(_active_presenter == NULL){
        activatePresenter(presenter);
    }

    return idx;
}

void BUI::activatePresenter(BUIPresenter *presenter){
    presenter->init();
    _model.setPresenter(presenter);
    _active_presenter = presenter;
}

void BUI::activatePresenterByIdx(uint8_t idx){
    struct list_node *head = &_presenter_list;

    while(idx--){
        head = head->next;
        if(head == NULL){
            return;
        }
    }

    activatePresenter((BUIPresenter*)head->elem);
}

void BUI::activateNextPresenter(void){
    // Search node
    struct list_node *head = &_presenter_list;
    BUIPresenter *pre;

    while(head->elem != _active_presenter){
        if(head->next == NULL){
            return;
        }
        head = head->next;
    }
    
    if(head->next != NULL){
        pre = (BUIPresenter *)head->next->elem;
    }else{
        pre = (BUIPresenter *)_presenter_list.elem;
    }

    activatePresenter(pre);
}

uint16_t listInsert(struct list_node *head, void *elem){

    uint8_t idx = 0;

    // Check for empty list
    if(head->elem == NULL){
        head->elem = (struct list_node*)elem;
        return idx;        
    }

    idx++;

    struct list_node *node = (struct list_node*)bui_malloc(sizeof(struct list_node));

    if(node == NULL){
        // Unable to allocate
        return 0xFFFF;
    }

    node->elem = (struct list_node*)elem;
    node->next = NULL;

    while(head->next != NULL){
        head = head->next;
        idx++;
    }

    head->next = node;

    return idx;
}
