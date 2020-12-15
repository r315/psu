#include "cmdpwm.h"
#include "psu.h"


void CmdPwm::help(void){ 
    console->print("\nUsage: pwm <ch> <command> [parameters]\n");
    console->print("PWM control commands\n\n");
    console->print("Channel\t: 1-3\n\n");
    console->print("val\t: set channel value, [value] 0-1024\n");    
    console->print("\tControl mode (no value specified on command):\n");
    console->print("\t\t+/-, change duty\n");
    console->print("\t\tv, enter value\n");
    console->print("\t\ts, stop\n\n");
    console->print("cfg <min> <max>\t: Configure pwm range\n\n");
    console->print("Pins:\n"
                "\tPWM1 (PB4): %d\n"
                "\tPWM2 (PB5): %d\n"
                "\tPWM3 (PB0): %d\n", PWM_Get(0), PWM_Get(1), PWM_Get(2));
}

char CmdPwm::execute(void *ptr){
    char *str = (char*)ptr, c;
    int32_t ch, val;
    char line[5];

    if(!nextInt(&str, &ch)){
        help(); 
        return CMD_BAD_PARAM;
    }

    if(ch < 1 || ch > 3){
        help(); 
        return CMD_BAD_PARAM;
    }

    ch--;

    if(isNextWord(&str, "val")){
        if(nextInt(&str, &val)){
            PWM_Set(ch, val);
        }else{
            val = PWM_Get(ch);
            while( (c = console->xgetchar()) != 'q'){
                switch(c){
                    case '+':
                        if(val < PWM_MAX_VALUE){
                            PWM_Set(ch, ++val);
                        }
                        console->print("\r%u", val);
                        break;
                    case '-':
                        if(val > PWM_MIN_VALUE){
                            PWM_Set(ch, --val);
                        }
                        console->print("\r%u", val);
                        break;
                    case 'v':
                        console->print("\rCH[%u]=", ch + 1);  // restore channel number
                        console->getLine(line, sizeof(line));
                        if(yatoi(line, (int32_t*)&val) == 0){
                            console->print("\rInvalid value [%d - %d]", PWM_MIN_VALUE, PWM_MAX_VALUE);
                        }else{
                            PWM_Set(ch, val);
                        }
                        break;
                }       
            }
            console->xputchar('\n');
        }
        return CMD_OK;       
    }

    if(isNextWord(&str, "cfg")){        
        int32_t min, max;        
        if(nextInt(&str, &min)){
            if(nextInt(&str, &max)){
                psu_setPwmChannelCalibration(ch, min, max);
                return CMD_OK;
            }
        }
        pwmcal_t pwm = psu_getPwmChannelCalibration(ch);
        console->print("Min: %u, Max %u\n", pwm.min, pwm.max);
        return CMD_OK;        
    }

    help();
    return CMD_BAD_PARAM;
}
