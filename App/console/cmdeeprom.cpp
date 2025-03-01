#include "cmdeeprom.h"
#include "cmdutil.h"
#include "eeprom.h"
#include "psu.h"

#ifdef ENABLE_EEPROM
void CmdEeprom::dumpAddress(uint16_t addr){
    uint8_t buf[16];

    if(!EEPROM_Read(PSU_I2C_BUS, addr, buf, 16)){
        return;
    }

    console->printf("%02X ",addr);

    for(uint8_t i = 0; i < 16; i++){
        if((i&7) == 0){
            console->printchar(' ');
        }
        console->printf("%02X ",buf[i]);
    }

    console->print(" |");

	for(uint8_t i=0; i < 16; i++){
		if(buf[i] > (' '-1) && buf[i] < 0x7F)
			console->printchar(buf[i]);
		else{
			console->printchar(' ');
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

char CmdEeprom::execute(int argc, char **argv){
    int32_t addr, data;

    if(argc == 1){
        help();
        return CMD_OK;
    }

    if(!xstrcmp(argv[1], "dump")){
        console->printchar('\n');
	    for(uint16_t i = 0; i < EEPROM_SIZE ; i += 16){
		    dumpAddress(i);
	    }
        console->printchar('\n');
        return CMD_OK;
    }

    if(!xstrcmp(argv[1], "init")){
        //EEPROM_Erase();
        return CMD_OK;
    }

    if(!xstrcmp(argv[1], "erase")){
        EEPROM_Erase(PSU_I2C_BUS);
        return CMD_OK;
    }

    if(!xstrcmp(argv[1], "save")){
        app_saveState();
        return CMD_OK;
    }

    if(!xstrcmp(argv[1], "r")){
        if(ha2i(argv[2], (uint32_t*)&addr)){
            EEPROM_Read(PSU_I2C_BUS, addr, (uint8_t*)&data, 1);
            console->printf("%02X\n", data);
            return CMD_OK;
        }
    }

    if(!xstrcmp(argv[1], "w")){
        if(ha2i(argv[2], (uint32_t*)&addr)){
            if(ha2i(argv[3], (uint32_t*)&data)){
                EEPROM_Write(PSU_I2C_BUS, addr, (uint8_t*)&data, 1);
                return CMD_OK;
            }
        }
    }

    return CMD_BAD_PARAM;
}
#endif