
#include "command.h"
#include "strfunctions.h"


 // Called only once by default command object
Command::Command(Vcom *vc){
		memset(cmdList, 0 , COMMAND_MAX_CMD * sizeof(Command*));
		this->name = "help";
		this->vcom = vc;
}

void Command::help(void){
    
     vcom->puts("Available commands:\n\n");
    // first entry is this help
    for (uint8_t i = 1; i < COMMAND_MAX_CMD; i++){
        if(cmdList[i] != NULL)
            vcom->printf("\t%s\n",cmdList[i]->getName());
    }
    vcom->putc('\n');
}

void Command::add(Command *cmd){
    for (uint8_t i = 0; i < COMMAND_MAX_CMD; i++){
        if (cmdList[i] == NULL){
            cmdList[i] = cmd;
            return;
        }
    }
    vcom->printf("Command list full!");
}

char Command::parse(char *line){
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
        vcom->printf("Command not found\n");
    }else if(res == CMD_BAD_PARAM){ 
        vcom->printf("Bad parameter \n");
    }

    return res;
}

char Command::execute(void *ptr){
    help();
    return CMD_OK; 
}


/*
char cmdExecute(char *line, CmdLine *comdList, uint8_t listSize){
CmdLine *pcmd;
char *param, res;
uint8_t i;
  
    param = strtok_s(line, ' ', COMMAND_MAX_LINE, &line);

    if(param == NULL)
			param = (char*)"";

    pcmd = NULL;

    for(i = 0; i < listSize; i++, comdList++){
        if( xstrcmp((char*)comdList->name, param) == 0){
            pcmd = comdList;
            break;
        }
    }

    if(pcmd == NULL){
        res = CMD_NOT_FOUND;
    }
    else{
        res = pcmd->callCmd((void*)line);
    }
    return res;
}
*/