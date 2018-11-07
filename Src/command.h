#ifndef _command_h_
#define _command_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <string.h>

#include "vcom.h"
#include "strfunctions.h"


#define COMMAND_MAX_CMD  50
#define COMMAND_MAX_LINE 64

enum{
	CMD_OK,
    CMD_NOT_FOUND,
    CMD_BAD_PARAM
};

class Command{

private:


protected:
	const char *name;
public:
	const char *getName(void) { return this->name; }

	char *toString(void) { return (char*)this->name; }

	char isNameEqual(char *cmd){ return xstrcmp(cmd, (char*)this->name) == 0; }

	char execute(void *ptr);
	void help(void);

    Command(const char *nm){ this->name = nm; }
};


#ifdef __cplusplus
}
#endif

#endif