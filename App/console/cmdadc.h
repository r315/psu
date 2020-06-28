#ifndef _cmd_adc_h_
#define _cmd_adc_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <console.h>

class CmdAdc : public ConsoleCommand{
    Console *console;
public:
    void init(void *params) { console = static_cast<Console*>(params); }
    CmdAdc () : ConsoleCommand("adc") { }
    char execute(void *ptr);
    void help(void);
private:
    void printResolution(void);
    void printCalibrationData(void);
    void printChannelVoltage(uint8_t channel);
};


#ifdef __cplusplus
}
#endif

#endif