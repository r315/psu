#
#	MAKEFILE FOR HI-TECH COMPILER
#	MCU = PIC16F73
#   04-06-2012 makefile sem configuração


#SRC = $(wildcard *.c)
SRC = ctrlbd.c
#MCU = 16F819
MCU = 16F76
CC = picc.exe

CFLAGS = -q -g -P --char=unsigned --asmlist --chip=$(MCU) 
CFLAGS +="--errformat=%f:%l: Error: %s"  
CFLAGS +="--warnformat=%f:%l: Warning: %s"
CFLAGS += -I"..\..\..\..\embedded\pic\include"

%.obj: %.c
	$(CC) $(CFLAGS) -c $<	
	
%.hex: %.obj
	$(CC) $(CFLAGS) $<

all: $(SRC:.c=.hex)
	
clean:
	rm -rf *.obj *.rlf *.sdb *.lst *.cof *.hxl *.map *.sym *.hex

	
	
	

