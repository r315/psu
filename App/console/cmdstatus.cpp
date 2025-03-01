#include "psu.h"
#include "cmdstatus.h"

void CmdStatus::help(void){
    console->print("System status\n");
}

char CmdStatus::execute(int argc, char **argv){
    console->print("\n----------------------------------\n");
    console->printf("PSU %s\n", PSU_VERSION);
    console->print("----------------------------------\n");
    console->printf("Free mem:\t%u bytes\n", xPortGetFreeHeapSize());
    console->print("----------------------------------\n");
    console->printf("Input Voltage:\t%umV\n", psu_getInputVoltage());
    console->print("----------------------------------\n");
    console->printf("Output voltage:\t%umV\n", psu_getOutputVoltage());
    console->printf("Output current:\t%umA\n", psu_getOutputCurrent());
    console->printf("Output enable:\t%s\n", app_isOutputEnabled() ? "ON" : "OFF");
    console->print("----------------------------------\n");
    console->printf("Load Voltage:\t%umV\n", psu_getLoadVoltage());
    console->printf("Load Current:\t%umA\n", psu_getLoadCurrent());
    console->print("----------------------------------\n");
    console->printf("USB current:\t%umA\n", psu_getUsbCurrent());
    console->print("----------------------------------\n");
    console->printf("ADC Resolution:\t%.2fmv\n", ADC_GetResolution());
    console->print("----------------------------------\n");
    console->printf("Preset idx:\t%d\n", app_getPresetIdx());
    console->print("----------------------------------\n\n");
    return CMD_OK;
}