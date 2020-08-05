#include "cmdadc.h"
#include "board.h"

char *getOptValue(const char *opt, uint32_t argc, char **argv);

void CmdAdc::help(void){ 
    console->print("\nUsage: adc <command>\n");
    console->print("Adc configuration\n\n");
    console->print("Commands:\n");
    console->print("\tres\t\tResolution\n");
    console->print("\tcal\t\tCalibrate ADC\n");
    console->print("\tread <ch|all>\tread channel value\n");
    //console->print("\tplot <ch>\tconstantly plot channel value in to graph, 'q' to stop\n\n");
    console->print("\tgain <ch> <float>\tset channel gain\n");
}

void CmdAdc::printResolution(void){
    console->print("Resolution: %.2fmv\n", ADC_GetResolution());
}

void CmdAdc::printCalibrationData(void){
    console->print("Calibration data: %u\n", ADC_GetCalibration());
}

void CmdAdc::printChannelVoltage(uint8_t channel){
    console->print("CH %u: %.2fmv\n", channel, ADCMGR_GetChannelVoltage(channel));
}

char CmdAdc::execute(void *ptr){ 
char *argv[4], *param;
uint32_t argc;
int32_t intvalue;
    
    argc = strToArray((char*)ptr, argv);

    if(argc == 0){
        help();
        printCalibrationData();
        printResolution();
        return CMD_OK;
    }

    if((param = getOptValue("res", argc, argv)) != NULL){
        printResolution();
        return CMD_OK;
    }

    if((param = getOptValue("cal", argc, argv)) != NULL){
        ADC_Calibrate();
        printCalibrationData();        
        return CMD_OK;
    }

    if((param = getOptValue("read", argc, argv)) != NULL){
        if(nextInt(&param, &intvalue)){
            printChannelVoltage(intvalue);
            return CMD_OK;
        }

        if(!xstrcmp(param, "all")){
            for (size_t i = 0; i < ADCMGR_NUM_CHANNELS; i++)
            {
                printChannelVoltage(i);
            }
            return CMD_OK;
        }
    }

    return CMD_BAD_PARAM;
}