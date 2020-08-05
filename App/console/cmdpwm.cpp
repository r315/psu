#include "cmdpwm.h"
#include "board.h"

void CmdPwm::help(void){ 
    console->print("\nUsage: pwm <ch> [value]\n");
    console->print("PWM control\n\n");
    console->print("Channel: 1-4\n");
    console->print("Value: 0-1023\n");
    console->print("Control mode (no value specified on command):\n\n");
    console->print("\t+/-, change duty\n");
    console->print("\tv, enter value\n");
    console->print("\tq, quit\n\nPins:\n");
    console->print(
                "\tPWM1 (PB4): %d\n"
                "\tPWM2 (PB5): %d\n"
                "\tPWM3 (PB0): %d\n"
                "\tPWM4 (PB1): %d\n", PWM_Get(0), PWM_Get(1), PWM_Get(2), PWM_Get(3));
}

char CmdPwm::execute(void *ptr){
int32_t channel, curValue;
char line[5];

    if(!nextInt((char**)&ptr, &channel) || channel < 1 || channel > 4){
        help(); 
        return CMD_BAD_PARAM;
    }

    channel--;  // adjust to index    

    if(!nextInt((char**)&ptr, &curValue)){
        curValue = PWM_Get(channel);
    }else{
        PWM_Set(channel, curValue);
        return CMD_OK;
    }

    uint8_t c = 0;

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
                console->print("\rCH[%u]=", channel + 1);  // restore channel number
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