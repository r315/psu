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
    console->print("\tread <ch|all>\tread channel [0-15] value\n");
    console->print("\tplot <ch>\tprint channel value for graph plot\n");
    console->print("\t\tch : 0-15, 16 prints channels 0,1 for I/V plot \n");
    console->print("\tgain <ch> [float]\tget/set channel gain\n");
}

void CmdAdc::printResolution(void){
    console->printf("Resolution: %.2fmv\n", ADC_GetResolution());
}

void CmdAdc::printCalibrationData(void){
    console->printf("Calibration data: %u\n", ADC_GetCalibration());
}

void CmdAdc::printChannelVoltage(uint8_t channel){
    if(channel < AN_MUX_NUM_CH){
        console->printf("CH %u:\t%umv\n", channel, psu_getChannelVoltage(channel));
    }
}

char CmdAdc::execute(int argc, char **argv){
int32_t intvalue;
double floatvalue;

    if(!xstrcmp(argv[1], "res")){
        printResolution();
        return CMD_OK;
    }

    if(!xstrcmp(argv[1], "cal")){
        ADC_Calibrate();
        printCalibrationData();
        return CMD_OK;
    }

    if(!xstrcmp(argv[1], "read")){
        if(ia2i(argv[2], &intvalue)){
            printChannelVoltage(intvalue);
            return CMD_OK;
        }

        for (size_t i = 0; i < AN_MUX_NUM_CH; i++){
            printChannelVoltage(i);
        }
        return CMD_OK;
    }

    if(!xstrcmp(argv[1], "plot")){
        if(ia2i(argv[2], &intvalue)){
            static TickType_t xLastWakeTime;
            while( !console->available() ){
                uint32_t count = g_mgr_eoc_count;
                if(intvalue < 16){
                    console->printf("%d\r", psu_getChannelVoltage(intvalue));
                }else{
                    console->printf("%d,%d\r", psu_getChannelVoltage(0), psu_getChannelVoltage(1));
                }
                while(count == g_mgr_eoc_count){
                    vTaskDelayUntil( &xLastWakeTime, pdMS_TO_TICKS(50));
                }
            }
            return CMD_OK;
        }
    }

    if(!xstrcmp(argv[1], "gain")){
        if(ia2i(argv[2], &intvalue)){
            if(da2d(argv[3], &floatvalue)){
                psu_setChannelGain(intvalue, floatvalue);
            }else{
                console->printf("%.2f\n", psu_getChannelGain(intvalue));
            }
            return CMD_OK;
        }
    }

    help();
    printCalibrationData();
    printResolution();

    return CMD_BAD_PARAM;
}