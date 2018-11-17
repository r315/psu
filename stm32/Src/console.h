#ifndef _console_h_
#define _console_h_


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "command.h"
#include "vcom.h"

#define NO        0
#define YES       1

#define LINE_MAX_LEN    64


class Console : public Command
{
    Command *cmdList[COMMAND_MAX_CMD];    
    uint8_t executing;
    char line[LINE_MAX_LEN];
    uint8_t lineLen;
    Vcom vcom;
public:
    void init(void);
    char getLine(char *line, uint8_t max);
    char getLineNonBlocking(char *line, uint8_t *curLen, uint8_t max);
    void process(void);

    char execute(void *ptr);
	void help(void);

    void addCommand(Command *cmd);
    char parseCommand(char *line, uint8_t len);
    char executeCommand(void *ptr);
    void puts(const char* str);
    char getchar(void);
    void gets(char* str);
    char getline(char *line, uint8_t max);
    void print(const char* str, ...);

    Console(void) : Command("help"){ init(); }
};


extern Console console;


#ifdef __cplusplus
}
#endif

#endif