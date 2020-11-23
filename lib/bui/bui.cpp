#include "bui.h"
#include "lcd.h"
#include "font.h"


void BUI::handler(void *ptr){   
    
    if(_screen == NULL){
        return;
    }

    BUIView *view = _screen->view;
    BUIPresenter *presenter = _screen->presenter;
    buievt_t evt;

    if(view == NULL || presenter == NULL){
        return;
    }

    // read input
    if(BUTTON_Read() != BUTTON_EMPTY){
        evt.key = BUTTON_GetValue();
        evt.type = BUTTON_GetEvents();        
        presenter->eventHandler(&evt);
    }

    if(view->isSuspending()){
        ActivateNextScreen();
        view->clrFlag(BUI_FLAG_SUSPEND);
        dbg_printf("Suspending\n");

        //change presenter on model
        return;
    }

    // Check invalidated elements
    struct list_node *node = view->getWidgets();

    while(node != NULL){
        BUIWidget *wi = (BUIWidget*)node->elem;
        if(wi != NULL){
            wi->draw();
        }
        node = node->next;            
    }
    // update display    
}

BUI::BUI(BUIModel &m) : _model(m){
    _screen = NULL;
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
uint8_t BUI::createScreen(BUIView *view, BUIPresenter *presenter){

    buiscreen_t *scr = (buiscreen_t*)bui_malloc(sizeof(buiscreen_t));

    if(scr == NULL){
        return -1;
    }

    scr->view = view;
    scr->presenter = presenter;

    presenter->setView(view);
    presenter->setModel(&_model);
    
    uint8_t idx = listInsert(&_scrlist, (void *)scr);
    
    // If no active screen, set current as active
    if(_screen == NULL){
        _model.setPresenter(presenter);
        _screen = scr;
    }

    return idx;
}

void BUI::setActiveScreen(uint8_t idx){
    /*struct list_node *head = &_scrlist;
    while(idx--){
        head = head->next;
    }

    _screen = (buiscreen_t*)head->elem;*/
}

void BUI::ActivateNextScreen(void){
    
    /*if(_screen->next != NULL){
        _screen = _screen->next;
    }else{
        _screen = &_scrlist;
    }*/
}

uint16_t listInsert(struct list_node *head, void *elem){

    uint8_t idx = 0;

    // Check for empty list
    if(head->elem == NULL){
        head->elem = (struct list_node*)elem;
        return idx;        
    }

    struct list_node *node = (struct list_node*)bui_malloc(sizeof(struct list_node));

    if(node == NULL){
        // Unable to allocate
        return 0;
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
