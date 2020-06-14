#include "cmdset.h"
#include "strfunc.h"
#include "psu.h"

char *getOptValue(const char *opt, uint32_t argc, char **argv){
    for(uint32_t i = 0; i < argc; i++){
        if(xstrcmp(opt, argv[i]) == 0){
            return argv[i+1];
        }
    }
    return NULL;
}

int readFloatParameter(const char *opt, uint32_t argc, char **argv, uint16_t *dst, void (*func)(float)){
    union {float f; uint32_t u; } t;

    char *param = getOptValue((char*)opt, argc, argv);
    double d;
    
    if(param == NULL){
        return CMD_NOT_FOUND;
    }		

    if(nextDouble(&param, &d) == 0){
        return CMD_BAD_PARAM;
    }

    t.f = d;		
    *dst = (uint16_t)t.u;
    *(dst + 1) = (uint16_t)(t.u>>16);
    func(t.f);

    return CMD_OK;	
}

void dummy(float val){
    DBG_PRINT("%s: %f\n", __FUNCTION__, val);
}

void CmdSet::help(void){
    console->print("Usage: set <command> <parameter>\n");
    console->print("Psu control commands\n\n");
    console->print("Commands:\n");
    console->print("\tvoltage <float>\t\tSet output voltage\n");
    console->print("\tcurrent <float>\t\tSet output current limit\n");
    console->print("\toutput <on|off>\t\tEnable output\n");
}

char CmdSet::execute(void *ptr){
    char *argv[4], *param;
    uint32_t argc;
    argc = strToArray((char*)ptr, argv);
    uint16_t value;

    if(argc == 0){
        help();
        return CMD_OK;
    }

    if((param = getOptValue("output", argc, argv)) != NULL){
        if(xstrcmp("on", param) == 0){
            psu_setOutputEnable(true);
        }else if(xstrcmp("off", param) == 0){
            psu_setOutputEnable(false);            
        }else{
            console->print("output: %s\n", psu_getOutputEnable()?"on":"off");
        }
        return CMD_OK;
    }

    if(readFloatParameter("voltage", argc, argv, &value, dummy) == CMD_OK)
        return CMD_OK;
    if(readFloatParameter("current", argc, argv, &value, dummy) == CMD_OK)
        return CMD_OK;

    return CMD_BAD_PARAM;
}