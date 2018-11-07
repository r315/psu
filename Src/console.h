#ifndef _console_h_
#define _console_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "command.h"
#include "vcom.h"

class Console{
    Command *cmdList[COMMAND_MAX_CMD];
public:
    Vcom vcom;
    void init(void);
    char getLine(char *line, uint8_t max);
    void printf (const char* str, ...);

    void addCommand(Command *cmd);
    char parseCommand(char *line);
    char executeCommand(void *ptr);
    void puts(const char* str);
    char getchar(void);
    void gets(char* str);
    char getline(char *line, uint8_t max);
    void print(const char* str, ...);
};


extern Console console;


#ifdef __cplusplus
}
#endif

#endif