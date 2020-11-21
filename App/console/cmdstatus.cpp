#include "psu.h"
#include "cmdstatus.h"

void CmdStatus::help(void){
    console->print("System status\n");
}

char CmdStatus::execute(void *ptr){
    console->print("\n----------------------------------\n");
    console->print("Free mem:\t%u bytes\n", xPortGetFreeHeapSize());
    console->print("----------------------------------\n");
    console->print("Input Voltage:\t%umV\n", psu_getInputVoltage());
    console->print("----------------------------------\n");
    console->print("Output voltage:\t%umV\n", psu_getOutputVoltage());
    console->print("Output current:\t%umA\n", psu_getOutputCurrent());
    console->print("Output enable:\t%s\n", app_isOutputEnabled() ? "ON" : "OFF");
    console->print("----------------------------------\n");    
    console->print("Load Voltage:\t%umV\n", psu_getLoadVoltage());
    console->print("Load Current:\t%umA\n", psu_getLoadCurrent());
    console->print("----------------------------------\n");
    console->print("USB current:\t%umA\n", psu_getUsbCurrent());
    console->print("----------------------------------\n");
    console->print("ADC Resolution:\t%.2fmv\n", ADC_GetResolution());
    console->print("----------------------------------\n\n");
    return CMD_OK;
}