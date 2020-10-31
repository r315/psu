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
    console->print("\tgain <ch> <float>\tset channel gain\n");
    console->print("\tmgr <stop|resume|convert <ch>> | plot <ch>\n");
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

    if((param = getOptValue("mgr", argc, argv)) != NULL){
        if(!xstrcmp(param, "stop")){            
            app_enable_adcmgr(FALSE);
            console->print("ADC manager stopped.\n");
            return CMD_OK;
        }        

        if(!xstrcmp(param, "convert")){
            param = argv[2];
            if(nextInt(&param, &intvalue)){
                console->print("CH %u: %d\n", intvalue, ADCMGR_Convert(intvalue));
                return CMD_OK;
            }
        }

        if(!xstrcmp(param, "resume")){
            app_enable_adcmgr(TRUE);
            return CMD_OK;            
        }

    }

    if((param = getOptValue("plot", argc, argv)) != NULL){
        if(nextInt(&param, &intvalue)){
            static TickType_t xLastWakeTime;
            while( !console->kbhit() ){
                //console->print("%.2f\n", ADCMGR_GetChannelVoltage(intvalue)/1000);
                console->print("%d\n", ADCMGR_Convert(intvalue));
                vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(200));
            }
            return CMD_OK;
        }
    }

    return CMD_BAD_PARAM;
}