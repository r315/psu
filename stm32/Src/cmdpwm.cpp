
#include "common.h"

void CmdPwm::help(void){ 
console->print("\tusage pwm <ch>\n\tch, 1-4\n");
console->print("\t+/-, duty\n");
//console->print("\ts, stop\n");
console->print("\tq, quit\n");
console->print("\tPins\n"
                "\t\tPA6 PWM1\n"
                "\t\tPA7 PWM2\n"
                "\t\tPB0 PWM3\n"
                "\t\tPB1 PWM4\n");
}

char CmdPwm::execute(void *ptr){
uint32_t channel;

    if(!nextHex((char**)&ptr, &channel) || channel < 1 || channel > 4){
        help(); 
        return CMD_BAD_PARAM;
    }

    channel--;  // adjust to index    

    uint8_t c = 0;
    uint16_t curValue = PWM_Get(channel);
    while( (c = console->getchar()) != 'q'){
        switch(c){
            case '+':
                if(curValue < PWM_MAX_VALUE)
                    PWM_Set(channel, ++curValue);
                break;
            case '-':
                if(curValue > PWM_MIN_VALUE)
                    PWM_Set(channel, --curValue);
                break;
            case 's':

                break;
        }       
    }
    console->putc('\n');
    return CMD_OK; 
}