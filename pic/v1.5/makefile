###############################################
#GENERIC PIC PROJECT MAKEFILE
###############################################
MCU = 16F73
#MCU = 16F819
PRJ = psu
SRC = src/psu.c
#SRC = $(wildcard *.c)
INC = -I../../../../Embedded/PIC/include -Iinclude

###############################################
#general flags
CC = picc.exe
CFLAGS = -q -g -P --char=unsigned --asmlist --chip=$(MCU) 
CFLAGS +="--errformat=%f:%l: Error: %s"  
CFLAGS +="--warnformat=%f:%l: Warning: %s" $(INC)
all:	
	$(CC) $(CFLAGS) -C $(SRC)
#$(CC) $(CFLAGS) -o$(SRC:.c=.cof) -m$(SRC:.c=.map) $(SRC:.c=.obj)
	$(CC) $(CFLAGS) -o$(PRJ).cof $(PRJ).obj
#$(CC) $(CFLAGS) -o$(PRJ).cof lcdtest.obj
clean:
	rm -rf *.obj *.rlf *.sdb *.lst *.cof *.hxl *.map *.sym *.hex
