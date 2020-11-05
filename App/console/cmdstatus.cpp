#include "psu.h"
#include "cmdstatus.h"

void CmdStatus::help(void){
    console->print("System status\n");
}

char CmdStatus::execute(void *ptr){
    console->print("\n----------------------------------\n");
    console->print("Input Voltage:\t%umV\n", psu_getVin());
    console->print("----------------------------------\n");
    console->print("Output voltage:\t%umV\n", psu_getVoltage());
    console->print("Output current:\t%umA\n", psu_getCurrent());
    console->print("Output enable:\t%s\n", psu_getOutputEnable() ? "ON" : "OFF");
    console->print("----------------------------------\n");    
    console->print("Load Voltage:\t%umV\n", psu_getLoadVoltage());
    console->print("Load Current:\t%umA\n", psu_getLoadCurrent());
    console->print("----------------------------------\n");
    console->print("USB current:\t%umA\n", psu_getCurrentUSB());
    console->print("----------------------------------\n");
    console->print("ADC Resolution:\t%.2fmv\n", ADC_GetResolution());
    console->print("----------------------------------\n\n");
    return CMD_OK;
}