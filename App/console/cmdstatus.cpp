#include "psu.h"
#include "cmdstatus.h"

void CmdStatus::help(void){
    console->print("System status\n");
}

char CmdStatus::execute(void *ptr){
    console->print("\nOutput voltage: %umV\n", psu_getVoltage());
    console->print("Output current: %umA\n", psu_getCurrent());
    console->print("Output enable: %s\n", psu_getOutputEnable() ? "ON" : "OFF");
    console->print("Resolution: %.2fmv\n\n", ADC_GetResolution());
    //console->print("Shunt resistor:\n");

    return CMD_OK;
}