#include "cmdpwm.h"
#include "board.h"

void CmdPwm::help(void){ 
console->print("\tusage pwm <ch>\n\tch, 1-4\n");
console->print("\t+/-, duty\n");
console->print("\tv, enter value\n");
console->print("\tq, quit\n");
console->xputs("\tPins\n"
                "\t\tPB4 PWM1 -> vset\n"
                "\t\tPB5 PWM2 -> iset\n"
                "\t\tPB0 PWM3 -> iload\n"
                "\t\tPB1 PWM4 -> NA");
}

char CmdPwm::execute(void *ptr){
uint32_t channel;
char line[5];

    if(!nextHex((char**)&ptr, &channel) || channel < 1 || channel > 4){
        help(); 
        return CMD_BAD_PARAM;
    }

    channel--;  // adjust to index    

    uint8_t c = 0;
    uint16_t curValue = PWM_Get(channel);
    while( (c = console->xgetchar()) != 'q'){
        switch(c){
            case '+':
                if(curValue < PWM_MAX_VALUE){
                    PWM_Set(channel, ++curValue);
                }
                console->print("\r%u", curValue);
                break;
            case '-':
                if(curValue > PWM_MIN_VALUE){
                    PWM_Set(channel, --curValue);
                }
                console->print("\r%u", curValue);
                break;
            case 'v':
                console->print("\rCH[%u]=", channel);
                console->getLine(line, sizeof(line));
                if(yatoi(line, (int32_t*)&curValue) == 0){
                    console->print("\rInvalid value [%d - %d]", PWM_MIN_VALUE, PWM_MAX_VALUE);
                }else{
                    PWM_Set(channel, curValue);
                }
                break;
        }       
    }
    console->xputchar('\n');
    return CMD_OK; 
}