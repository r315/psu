#include "cmdadc.h"
#include "board.h"

char *getOptValue(const char *opt, uint32_t argc, char **argv);

void CmdAdc::help(void){ 
    console->print("Usage: adc <command>\n");
    console->print("Adc configuration\n\n");
    console->print("Commands:\n");
    console->print("\tres\t\tResolution\n");
    console->print("\tcal\t\tCalibration data\n");
}

char CmdAdc::execute(void *ptr){ 
char *argv[4], *param;
uint32_t argc;
uint16_t value;
    
    argc = strToArray((char*)ptr, argv);

    if(argc == 0){
        help();
        return CMD_OK;
    }

    if((param = getOptValue("res", argc, argv)) != NULL){
        float res = ADC_GetResolution();
        console->print("Resolution: %.2fmv\n", res);
        return CMD_OK;
    }

    if((param = getOptValue("cal", argc, argv)) != NULL){
        uint16_t res = ADC_GetCalibration();
        console->print("Calibration: %u\n", res);
        return CMD_OK;
    }

    return CMD_BAD_PARAM;
}