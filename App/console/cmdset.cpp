#include "cmdset.h"
#include "strfunc.h"
#include "psu.h"
#include "cmdutil.h"

void CmdSet::help(void){
    console->print("Usage: set <parameter> <value>\n");
    console->print("PSU parameters\n\n");
    console->print("Parameters:\n");
    console->print("\tvoltage <mv>\t\tSet output voltage\n");
    console->print("\tcurrent <ma>\t\tSet output current limit\n");
    console->print("\tload <ma>\t\tSet load current\n");
    //console->print("\trs <float>\t\tSet current sense resistor value\n");
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
            app_setOutputEnable(true);
        }else if(xstrcmp("off", param) == 0){
            app_setOutputEnable(false);            
        }else{
            console->print("output: %s\n", app_isOutputEnabled()?"on":"off");
        }
        return CMD_OK;
    }

    if(readIntParameter("voltage", argc, argv, &value, psu_setOutputVoltage) == CMD_OK)
        return CMD_OK;
    if(readIntParameter("current", argc, argv, &value, psu_setOutputCurrent) == CMD_OK)
        return CMD_OK;
    if(readIntParameter("load", argc, argv, &value, psu_setLoadCurrent) == CMD_OK)
        return CMD_OK;

    return CMD_BAD_PARAM;
}