#ifndef _cmd_eeprom_h_
#define _cmd_eeprom_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <console.h>

class CmdEeprom : public ConsoleCommand{
    Console *console;
    void dumpAddress(uint16_t addr);
    
public:
    void init(void *params) { console = static_cast<Console*>(params); }
    CmdEeprom () : ConsoleCommand("eeprom") { }
    char execute(void *ptr);
    void help(void);
};

#ifdef __cplusplus
}
#endif

#endif