#include "cmdeeprom.h"
#include "cmdutil.h"
#include "eeprom.h"
#include "psu.h"

#ifdef ENABLE_EEPROM
void CmdEeprom::dumpAddress(uint16_t addr){
    uint8_t buf[16];

    if(!EEPROM_Read(addr, buf, 16)){
        return;
    }

    console->print("%02X ",addr);

    for(uint8_t i = 0; i < 16; i++){
        if((i&7) == 0){
            console->xputchar(' ');
        }
        console->print("%02X ",buf[i]);
    }

    console->print(" |");

	for(uint8_t i=0; i < 16; i++){
		if(buf[i] > (' '-1) && buf[i] < 0x7F)
			console->xputchar(buf[i]);
		else{
			console->xputchar(' ');
		}
	}

	console->print("|\n");
}


void CmdEeprom::help(void){
    console->print("Usage: eeprom <command> \n");
    console->print("EEPROM management\n\n");
    console->print("\tdump \t\tDisplay eeprom content\n");
    console->print("\terase \t\tErase eeprom\n");
    //console->print("\tinit \t\tInitialize eeprom with default data\n");
    console->print("\tsave \t\tSave settings\n");
    console->print("\tr <addr> \t\tRead eeprom\n");
    console->print("\tw <addr> <data>\t\tWrite to eeprom\n");
}

char CmdEeprom::execute(void *ptr){
    int32_t addr, data;
    char *args = (char*)ptr;  
    
    if(*args == '\0'){
        help();
        return CMD_OK;
    }

    if(!argcmp("dump", &args)){
        console->print("\n");
	    for(uint16_t i = 0; i < EEPROM_SIZE ; i += 16){
		    dumpAddress(i);
	    }
        console->print("\n");
        return CMD_OK;
    }

    if(!argcmp("init", &args)){
        //EEPROM_Erase();
        return CMD_OK;
    }

    if(!argcmp("erase", &args)){
        EEPROM_Erase();
        return CMD_OK;
    }

    if(!argcmp("save", &args)){
        app_saveState();
        return CMD_OK;
    }
    
    if(!argcmp("r", &args)){
        if(nextHex(&args, (uint32_t*)&addr)){
            EEPROM_Read(addr, (uint8_t*)&data, 1);
            console->print("%02X\n", data);
            return CMD_OK;
        }
    }

    if(!argcmp("w", &args)){
        if(nextHex(&args, (uint32_t*)&addr)){
            if(nextHex(&args, (uint32_t*)&data)){
                EEPROM_Write(addr, (uint8_t*)&data, 1);            
                return CMD_OK;
            }
        }
    }   
    
    return CMD_BAD_PARAM;
}
#endif