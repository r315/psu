
#include "bui.h"
#include "view_preset.h"
#include "model_psu.h"
#include "presenter_preset.h"

void PresenterPreset::update(void){

}

void PresenterPreset::eventHandler(buievt_t *evt){
    // user input will end here
    if(evt->type != BUTTON_PRESSED){
        return;
    }

    switch(evt->key){
        case BUTTON_MODE:
            // switch screen
            _view.suspend();
            break;

        case BUTTON_UP:
           
            break;

        case BUTTON_DOWN:
            
            break;

        case BUTTON_OUT:
            
            break;

        default:
            break;
    }
   
}
