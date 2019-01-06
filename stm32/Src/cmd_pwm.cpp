
#include "common.h"

void CmdPwm::help(void){ 
console.print("pwm <ch>\n");
console.print("+/- duty\n");
console.print("q exit\n");
}

char CmdPwm::execute(void *ptr){
uint32_t channel;

    if(!nextHex((char**)&ptr, &channel)){
        help(); 
        return CMD_BAD_PARAM;
    }

    uint8_t c = 0;
    uint16_t curValue = PWM_Get(channel);
    while( (c = console.getchar()) != 'q'){
        switch(c){
            case '+':
                if(curValue < PWM_MAXVALUE)
                    PWM_Set(channel, ++curValue);
                break;
            case '-':
                if(curValue > 0)
                    PWM_Set(channel, --curValue);
                break;                
        }       
    }    
    return CMD_OK; 
}