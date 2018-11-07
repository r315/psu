
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


#include "vcom.h"
#include "console.h"
#include "command.h"
#include "strfunctions.h"

void Console::init(void){
		memset(cmdList, 0 , COMMAND_MAX_CMD * sizeof(Command*));	
		print("\rConsole Started\n");	
}

/**
void Console::help(void){
    
    puts("Available commands:\n\n");
    // first entry is this help
    for (uint8_t i = 1; i < COMMAND_MAX_CMD; i++){
        if(cmdList[i] != NULL)
            printf("\t%s\n",cmdList[i]->getName());
    }
    putc('\n');
}*/

void Console::addCommand(Command *cmd){
    for (uint8_t i = 0; i < COMMAND_MAX_CMD; i++){
        if (cmdList[i] == NULL){
            cmdList[i] = cmd;
            return;
        }
    }
    printf("Command list full!");
}

char Console::parseCommand(char *line){
char res = CMD_NOT_FOUND, *cmdname, *param;  
Command **cmd = cmdList;

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
        printf("Command not found\n");
    }else if(res == CMD_BAD_PARAM){ 
        printf("Bad parameter \n");
    }

    return res;
}

char Console::executeCommand(void *ptr){
    //help();
    return CMD_OK; 
}

/**
 * libc functions * 
 * */
void Console::puts(const char* str)
{
	while(*str != '\0')
	{
		vcom.putc(*str++);
	}
    vcom.putc('\n');
}

void Console::gets(char* str)
{
	char c;
	c = vcom.getc();
	while((c != '\n') && (c != '\r'))
	{
		*str++ = c;
		c = vcom.getc();
	}
	*str = '\0';
}



char Console::getchar(void)
{
	char c = vcom.getc();
	vcom.putc(c);
	return c;
}



char Console::getline(char *line, uint8_t max)
{
char s = 0;
	char c;
	c = vcom.getc();
	while((c != '\n') && (c != '\r'))
	{
		if(c == '\b'){
			if(s != 0){
				vcom.putc(c);
				vcom.putc(' ');
				vcom.putc(c);
				line--;
				s--;
			}
		}else{
			if(s < max){
				vcom.putc(c);
			   *line++ = c;
			   s++;
			}			
		}
		c = vcom.getc();
	}
	*line = '\0';
	return s;
}

void Console::print (const char* str, ...)
{
	va_list arp;
	int d, r, w, s, l,f;

	va_start(arp, str);

	while ((d = *str++) != 0) {
		if (d != '%') {
			vcom.putc(d); continue;
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
			puts(va_arg(arp, char*));
			continue;
		}
		if (d == 'c') {
			vcom.putc((char)va_arg(arp, int));
			continue;
		}		
		if (d == 'u') r = 10;
		if (d == 'd') r = -10;
		if (d == 'X' || d == 'x') r = 16; // 'x' added by mthomas in increase compatibility
		if (d == 'b') r = 2;
		if(d == 'f'){
			if(!f)
				w = 6;						// dafault 6 decimal places
			puts(pftoa(va_arg(arp, double), w));			
			continue;
		}	
		if (!r) break;
		if (s) w = -w;
		if (l) {
			puts(pitoa((long)va_arg(arp, long), r, w));
		} else {
			if (r > 0)
				puts(pitoa((unsigned long)va_arg(arp, int), r, w));
			else
				puts(pitoa((long)va_arg(arp, int), r, w));
		}
	}

	va_end(arp);
}


