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
        dbg_printf("Suspending\n");
        view->clrFlag(BUI_FLAG_SUSPEND);
        ActivateNextScreen();
        return;
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
uint8_t BUI::createScreen(BUIPresenter *presenter){

    buiscreen_t *scr = (buiscreen_t*)bui_malloc(sizeof(buiscreen_t));

    if(scr == NULL){
        return -1;
    }

    scr->view = &presenter->getView();
    scr->presenter = presenter;

    presenter->setModel(&_model);
    
    uint8_t idx = listInsert(&_scrlist, (void *)scr);
    
    // If no active screen, set current as active
    if(_screen == NULL){
        _model.setPresenter(presenter);
        _screen = scr;
        _screen->view->init();
    }

    return idx;
}

void BUI::activateScreen(buiscreen_t *scr){
    scr->view->init();
    scr->presenter->setModel(&_model);
    _model.setPresenter(scr->presenter);
}

void BUI::setActiveScreen(uint8_t idx){
    struct list_node *head = &_scrlist;

    while(idx--){
        head = head->next;
        if(head == NULL){
            return;
        }
    }

    _screen = (buiscreen_t *)head->elem;

    activateScreen(_screen);
}

void BUI::ActivateNextScreen(void){

    // Search node
    struct list_node *head = &_scrlist;
    while(head->elem != _screen){
        if(head->next == NULL){
            return;
        }
        head = head->next;
    }
    
    if(head->next != NULL){
        _screen = (buiscreen_t *)head->next->elem;
    }else{
        _screen = (buiscreen_t *)_scrlist.elem;
    }

    activateScreen(_screen);
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
