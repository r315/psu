##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [3.10.0-B14] date: [Fri Oct 23 21:36:48 BST 2020] 
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET = app_psu

######################################
# building variables
######################################
# debug build?
DEBUG =1

#######################################
# paths
#######################################
# source path

APP_SRC_DIR :=$(CURDIR)/App

ifeq ($(shell uname -s), Linux)
REPO_F1 :=STM32Cube_FW_F1_V1.8.0
REPOSITORY :=$(HOME)/STM32Cube/Repository/$(REPO_F1)
else
REPOSITORY :=$(REPO_F1)
endif

FREERTOS_DIR :=$(REPOSITORY)/Middlewares/Third_Party/FreeRTOS/Source

# firmware library path
PERIFLIB_PATH = 

# Build path
BUILD_DIR :=build

BUI_DIR :=./lib/bui

UI_DIR :=./ui
######################################
# sources
######################################
# C sources
C_SOURCES =  \
Src/main.c \
Src/stm32f1xx_it.c \
Src/system_stm32f1xx.c \
Src/stm32f1xx_hal_msp.c \
$(wildcard $(APP_SRC_DIR)/*.c) \
$(wildcard $(APP_SRC_DIR)/components/*.c) \
$(LIBEMB_PATH)/display/font.c \
$(LIBEMB_PATH)/button/button.c \
$(LIBEMB_PATH)/misc/strfunc.c \
$(LIBEMB_PATH)/misc/debug.c \
$(LIBEMB_PATH)/misc/pinName.c \
$(LIBEMB_PATH)/drv/tft/st7735.c \
$(LIBEMB_PATH)/display/lcd.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2c.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd_ex.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usb.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c \
$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c \
$(FREERTOS_DIR)/portable/GCC/ARM_CM3/port.c \
$(FREERTOS_DIR)/portable/MemMang/heap_4.c \
$(FREERTOS_DIR)/list.c \
$(FREERTOS_DIR)/queue.c \
$(FREERTOS_DIR)/tasks.c \
$(FREERTOS_DIR)/timers.c \
$(FREERTOS_DIR)/CMSIS_RTOS/cmsis_os.c \
$(BUI_DIR)/bui_draw.c \

# USB lib
#C_SOURCES += \
$(REPOSITORY)Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
$(REPOSITORY)Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c \
$(REPOSITORY)Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c \
$(REPOSITORY)Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c \
Src/usbd_cdc_if.c \
Src/usbd_desc.c \
Src/usbd_conf.c \
Src/usb_device.c \

CPP_SOURCES = \
$(LIBEMB_PATH)/console/console.cpp \
$(wildcard $(APP_SRC_DIR)/*.cpp) \
$(wildcard $(APP_SRC_DIR)/console/*.cpp) \
$(wildcard $(BUI_DIR)/*.cpp) \
$(wildcard $(UI_DIR)/**/*.cpp) \
# ASM sources
ASM_SOURCES =  \
startup/startup_stm32f103xb.s

######################################
# Includes
######################################

# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-IInc \
-ISrc \
-I$(APP_SRC_DIR) \
-I$(APP_SRC_DIR)/components \
-I$(APP_SRC_DIR)/console \
-I$(APP_SRC_DIR)/screen \
-I"$(LIBEMB_PATH)"/include \
-I$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Inc \
-I$(REPOSITORY)/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy \
-I$(REPOSITORY)/Middlewares/ST/STM32_USB_Device_Library/Core/Inc \
-I$(REPOSITORY)/Drivers/CMSIS/Device/ST/STM32F1xx/Include \
-I$(REPOSITORY)/Drivers/CMSIS/Include \
-I$(REPOSITORY)/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc \
-I$(FREERTOS_DIR)/include \
-I$(FREERTOS_DIR)/CMSIS_RTOS \
-I$(FREERTOS_DIR)/portable/GCC/ARM_CM3 \
-I$(BUI_DIR) \
-I$(UI_DIR)/model \
-I$(UI_DIR)/screen_psu \

######################################
# firmware library
######################################
PERIFLIB_SOURCES = 

#######################################
# binaries
#######################################
BINPATH = 
PREFIX = arm-none-eabi-
CC = $(BINPATH)$(PREFIX)gcc
CPP = $(BINPATH)$(PREFIX)g++
AS = $(BINPATH)$(PREFIX)gcc -x assembler-with-cpp
CP = $(BINPATH)$(PREFIX)objcopy
AR = $(BINPATH)$(PREFIX)ar
SZ = $(BINPATH)$(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m3

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi
#FLOAT-ABI =-u_printf_float

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F103xB \
-DUSE_ADCMGR \
-DCONSOLE_BLOCKING \
-DUSE_COURIER_FONT \
-DUSE_GROTESKBOLD_FONT \
-D_ENABLE_USB_CDC \
-DENABLE_UART \

# compile gcc flags
ifeq ($(DEBUG), 1)
OPT =-Og -g -gdwarf-2
C_DEFS +=-DENABLE_DEBUG
else
OPT =-Os
endif

ASFLAGS =$(MCU) $(AS_DEFS) $(AS_INCLUDES) -Wall -fdata-sections -ffunction-sections
CFLAGS =$(MCU) $(OPT) $(C_DEFS) $(C_INCLUDES) -Wall -fdata-sections -ffunction-sections -std=c99
CPPFLAGS =$(MCU) $(OPT) $(C_DEFS) $(C_INCLUDES) -Wall -fdata-sections -ffunction-sections

# Generate dependency information
#CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst))

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT =startup/STM32F103C8Tx_FLASH.ld
#LDSCRIPT =startup/f103c8tx_dfu.ld

# libraries
#-nostdlib, -nostartfiles: Missign reference to _init
#-specs=nosys.specs, lnosys, not libs: has same size
#-specs=nano.specs: has the small size

LIBS =-specs=nano.specs -nolibc
LIBDIR = 
LDFLAGS = $(MCU) -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: elf #$(BUILD_DIR)/$(TARGET).bin #$(BUILD_DIR)/$(TARGET).hex
#@echo $(OBJECTS)

size: 
	@echo "--- Size ---"
	$(SZ) -A -x $(BUILD_DIR)/$(TARGET).elf
	$(SZ) -B $(BUILD_DIR)/$(TARGET).elf

elf: $(BUILD_DIR)/$(TARGET).elf size

$(TARGET).cfg:
	@echo "Creating opencod configuration file"
	echo "interface jlink" >> $@
	echo "transport select swd" >> $@
	echo "source [find target/stm32f1x.cfg]" >> $@
	echo "adapter_khz 4000" >> $@

#use winusb driver
program: elf $(TARGET).cfg
	openocd -f $(TARGET).cfg -c "program $(BUILD_DIR)/$(TARGET).elf verify reset exit"
#openocd -f $(TARGET).cfg -c "program $(BUILD_DIR)/$(TARGET).bin 0x08001000 verify reset exit"

upload: $(BUILD_DIR)/$(TARGET).bin
	dfu-util -a 0 -s 0x08001000 -D $< -R

test:
	@echo $(CURDIR)
	@echo ""; $(foreach d, $(VPATH), echo $(d);)
#@echo $(C_SOURCES)
#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
#vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.obj)))
#vpath %.cpp $(sort $(dir $(CPP_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
#vpath %.s $(sort $(dir $(ASM_SOURCES)))

VPATH +=$(dir $(C_SOURCES)) $(dir $(CPP_SOURCES)) $(dir $(ASM_SOURCES))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@echo "Compiling  " $<
	@$(CC) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.obj: %.cpp Makefile | $(BUILD_DIR)
	@echo "Compiling  " $<
	@$(CPP) -c $(CPPFLAGS)  -fno-exceptions -fno-unwind-tables -fno-rtti $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	@echo "Assembling " $<
	@$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) #Makefile
	@echo "--- Linking ---"
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir -p $@		

#######################################
# clean up
#######################################
clean:
	-rm -fR .dep $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
#-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)
export GCC_COLORS='error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01'
# *** EOF ***
