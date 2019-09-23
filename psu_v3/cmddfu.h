#ifndef _cmd_dfu_h_
#define _cmd_dfu_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <console.h>

#define ENABLE_DFU_MAGIC *(((uint64_t*)&_estack) - 1) = 0xDEADBEEFCC00FFEEULL
#define DISABLE_DFU_MAGIC *(((uint64_t*)&_estack) - 1) = 0x0UL

extern uint32_t _estack;

class CmdDfu : public ConsoleCommand{
    Console *console;
public:
    void init(void *params) { DISABLE_DFU_MAGIC; console = static_cast<Console*>(params); }
    CmdDfu () : ConsoleCommand("dfu") { }
    char execute(void *ptr);
    void help(void);
};


#ifdef __cplusplus
}
#endif

#endif