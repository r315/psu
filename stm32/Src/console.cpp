
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


#include "vcom.h"
#include "console.h"
#include "command.h"
#include "strfunctions.h"

static const char prompt[] = {"psu >"};


void Console::init(void){
		memset(cmdList, 0 , COMMAND_MAX_CMD * sizeof(Command*));	
		executing = NO;
		addCommand(this);		
		print(prompt);
}

void Console::help(void){    
    puts("Available commands:\n\n");
    // first entry is this
    for (uint8_t i = 0; i < COMMAND_MAX_CMD; i++){
        if(cmdList[i] != NULL)
            print("\t%s\n",cmdList[i]->getName());
    }
    putchar('\n');
}

void Console::addCommand(Command *cmd){
    for (uint8_t i = 0; i < COMMAND_MAX_CMD; i++){
        if (cmdList[i] == NULL){
            cmdList[i] = cmd;
            return;
        }
    }
    print("Command list full!");
}

char Console::parseCommand(char *line, uint8_t len){
char res = CMD_NOT_FOUND, *cmdname, *param;  
Command **cmd = cmdList;

	if (len == 1) /* if only one char it can only be \n or \r */
		return CMD_OK;

    cmdname = strtok_s(line, ' ', COMMAND_MAX_LINE, &param);

    for (uint8_t i = 0; i < COMMAND_MAX_CMD; i++){
        if(*cmd == NULL){
            break;
        }
        if((*cmd)->isNameEqual(cmdname) != 0){
            res = (*cmd)->execute((void*)param);
            break;
        }
        cmd += 1;
    }

    if (res == CMD_NOT_FOUND){
        print("Command not found\n");
    }else if(res == CMD_BAD_PARAM){ 
        print("Bad parameter \n");
    }

    return res;
}

char Console::execute(void *ptr){
    help();
    return CMD_OK; 
}

void Console::process(void){
	if(getLineNonBlocking(line, &lineLen, LINE_MAX_LEN)){
		parseCommand(line, lineLen);
		print(prompt);
		lineLen = 0;
	}
}

/**
 * libc functions * 
 * */
void Console::puts(const char* str)
{
	vcom.puts(str);
    vcom.putchar('\n');
}

void Console::gets(char* str)
{
	char c;
	c = vcom.getchar();
	while((c != '\n') && (c != '\r'))
	{
		*str++ = c;
		c = vcom.getchar();
	}
	*str = '\0';
}

char Console::getchar(void)
{
	char c = vcom.getchar();
	vcom.putchar(c);
	return c;
}

char Console::getLineNonBlocking(char *line, uint8_t *curLen, uint8_t max){
char c;
uint8_t len = 0;

	if(vcom.getCharNonBlocking(&c)){
		len = *curLen;
		if(c == '\b'){
			if(len > 0){
				vcom.putchar(c);
				vcom.putchar(' ');
				vcom.putchar(c);
				len--;				
			}
		}else{
			if(len < max){
				vcom.putchar(c);
			   *(line + len) = c;
			   len++;
			}			
		}
		
		*curLen = len;
		
		if((c == '\n') || (c == '\r')){
			*(line+len) = '\0';			
		}else{
			len = 0;
		}		
	}
	return len;
}

char Console::getline(char *line, uint8_t max)
{
	uint8_t len = 0;
	char c;

	do{
		c = vcom.getchar();
		if(c == '\b'){
			if(len != 0){
				vcom.putchar(c);
				vcom.putchar(' ');
				vcom.putchar(c);
				line--;
				len--;
			}
		}else{
			if(len < max){
				vcom.putchar(c);
			   *line++ = c;
			   len++;
			}			
		}
	}while((c != '\n') && (c != '\r'));
	*line = '\0';
	return len;
}

void Console::print (const char* str, ...)
{
	va_list arp;
	int d, r, w, s, l,f;

	va_start(arp, str);

	while ((d = *str++) != 0) {
		if (d != '%') {
			vcom.putchar(d); continue;
		}
		d = *str++; w = r = s = l = 0;
		if(d == '.'){           
			d = *str++; f = 1; 
		}
		if (d == '0') {
			d = *str++; s = 1;
		}
		while ((d >= '0')&&(d <= '9')) {
			w += w * 10 + (d - '0');
			d = *str++;
		}		
		if (d == 'l') {
			l = 1;
			d = *str++;
		}
		if (!d) break;
		if (d == 's') {
			vcom.puts(va_arg(arp, char*));
			continue;
		}
		if (d == 'c') {
			vcom.putchar((char)va_arg(arp, int));
			continue;
		}		
		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'X' || d == 'x') r = 16; // 'x' added by mthomas in increase compatibility
		if (d == 'b') r = 2;
		if(d == 'f'){
			if(!f)
				w = 6;						// dafault 6 decimal places
			vcom.puts(pftoa(va_arg(arp, double), w));			
			continue;
		}	
		if (!r) break;
		if (s) w = -w;
		if (l) {
			vcom.puts(pitoa((long)va_arg(arp, long), r, w));
		} else {
			if (r > 0)
				vcom.puts(pitoa((unsigned long)va_arg(arp, int), r, w));
			else
				vcom.puts(pitoa((long)va_arg(arp, int), r, w));
		}
	}

	va_end(arp);
}


uint8_t Console::kbhit(void){
	return vcom.kbhit();
}

