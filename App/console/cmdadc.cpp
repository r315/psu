#include "psu.h"
#include "cmdadc.h"
#include "cmdutil.h"

char *getOptValue(const char *opt, uint32_t argc, char **argv);

void CmdAdc::help(void){ 
    console->print("\nUsage: adc <command>\n");
    console->print("Adc configuration\n\n");
    console->print("Commands:\n");
    console->print("\tres\t\tADC Resolution\n");
    console->print("\tcal\t\tCalibrate ADC\n");
    console->print("\tread <ch|all>\tread channel value\n");
    console->print("\tplot <ch>\tplot channel value in to graph, press any key to stop\n");
    console->print("\tgain <ch> [float]\tget/set channel gain\n");
}

void CmdAdc::printResolution(void){
    console->print("Resolution: %.2fmv\n", ADC_GetResolution());
}

void CmdAdc::printCalibrationData(void){
    console->print("Calibration data: %u\n", ADC_GetCalibration());
}

void CmdAdc::printChannelVoltage(uint8_t channel){
    console->print("CH %u:\t%umv\n", channel, psu_getChannelVoltage(channel));
}

char CmdAdc::execute(void *ptr){ 
char *str = (char*)ptr;
int32_t intvalue;
double floatvalue;
    
    if(isNextWord(&str, "res")){
        printResolution();
        return CMD_OK;
    }

    if(isNextWord(&str, "cal")){
        ADC_Calibrate();
        printCalibrationData();        
        return CMD_OK;
    }

    if(isNextWord(&str, "read")){
        if(nextInt(&str, &intvalue)){
            printChannelVoltage(intvalue);
            return CMD_OK;
        }

        if(isNextWord(&str, "all")){
            for (size_t i = 0; i < AN_MUX_NUM_CH; i++){
                printChannelVoltage(i);
            }
            return CMD_OK;
        }
    }

    if(isNextWord(&str, "plot")){
        if(nextInt(&str, &intvalue)){
            static TickType_t xLastWakeTime;
            while( !console->kbhit() ){
                console->print("%d\n", psu_getChannelVoltage(intvalue));
                vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(200));
            }
            return CMD_OK;
        }
    }

    if(isNextWord(&str, "gain")){
        if(nextInt(&str, &intvalue)){
            if(nextDouble(&str, &floatvalue)){
                psu_setChannelGain(intvalue, floatvalue);
            }else{
                console->print("%.2f\n", psu_getChannelGain(intvalue));
            }
            return CMD_OK;
        }
    }

    help();
    printCalibrationData();
    printResolution();

    return CMD_BAD_PARAM;
}