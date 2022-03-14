
PROJECT_NAME=mapletest

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src

KOS_PATH=/opt/toolchains/dc/kos
SCRAMBLE_EXEC=$(KOS_PATH)/utils/scramble/scramble
BIN2O_EXEC=$(KOS_PATH)/utils/bin2o/bin2o
KOS_GENROMFS = $(KOS_PATH)/utils/genromfs/genromfs

TARGET_ELF = $(BUILD_DIR)/$(PROJECT_NAME).elf
TARGET_BIN = $(BUILD_DIR)/$(PROJECT_NAME).bin 
SCRAMBLED_BIN = $(BUILD_DIR)/$(PROJECT_NAME).scrambled.bin
TARGET_ISO = $(BUILD_DIR)/$(PROJECT_NAME).iso
TARGET_CDI = $(BUILD_DIR)/$(PROJECT_NAME).cdi

ROMDISK_FOLDER=romdisk
ROMDISK_IMAGE=$(BUILD_DIR)/romdisk.img
ROMDISK_OBJECT=$(BUILD_DIR)/romdisk.o
ROMDISK_FILES = $(wildcard $(ROMDISK_FOLDER)/*)

ISO_FOLDER=iso_data
ISO_FILES = $(wildcard $(ISO_FOLDER)/*)

# INPUT_TEXTURE_DIR = textures
# INPUT_TEXTURES = $(wildcard $(INPUT_TEXTURE_DIR)/*.png $(INPUT_TEXTURE_DIR)/*.jpg)
# OUTPUT_TEXTURES = $(addprefix build/romdisk/,$(addsuffix .tex,$(basename $(INPUT_TEXTURES))))

DREAM_LIBS= -ltsunami -lparallax -ldcplib -lpng -ljpeg -lkmg -lz -lbz2 -lkosutils -lm -Wl,--gc-sections
CC=kos-cc
CXX=kos-c++
LD=kos-ld
AS=kos-as
AR=kos-ar
OBJCOPY=$(KOS_OBJCOPY)

SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
#SRCS := src/main.cpp
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

#INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_DIRS := ./src/
INC_FLAGS := $(addprefix -I,$(INC_DIRS)) -I./lib/

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP -g -std=c++11 -lstdc++

default: cdi

.PHONY: cdi elf
cdi: $(TARGET_CDI)
elf: $(TARGET_ELF)

$(TARGET_ELF): $(OBJS) $(ROMDISK_OBJECT)
	@echo "Linking $(TARGET_ELF)"
	$(CXX) $(OBJS) $(ROMDISK_OBJECT) -o $@ $(DREAM_LIBS) -Wl,-Ttext=0x8C010000 -fno-omit-frame-pointer

$(TARGET_BIN): $(TARGET_ELF)
	@echo "Converting $(TARGET_EXEC) to Binary"
	sh-elf-objcopy -R .stack -O binary $< $@

#
$(TARGET_ISO): $(SCRAMBLED_BIN) IP.BIN $(ISO_FILES)
	mkdir -p $(ISO_FOLDER)
	cp $(SCRAMBLED_BIN) $(ISO_FOLDER)/1ST_READ.BIN
	mkisofs -C 0,11702 -V $(PROJECT_NAME) -G IP.BIN -l -o $@ $(ISO_FOLDER)/

$(TARGET_CDI): $(TARGET_ISO)
	cdi4dc $< $@
	
# scramble binary
$(SCRAMBLED_BIN): $(TARGET_BIN)
	$(SCRAMBLE_EXEC) $< $@
	
# assembly
$(BUILD_DIR)/%.s.o: %.s
	@$(MKDIR_P) $(dir $@)
	@echo $(AS) $<
	$(AS) -c $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	@$(MKDIR_P) $(dir $@)
	@echo $(CC) $< 
	$(CC) -c $< -o $@ -Wl,-Ttext=0x8CE00000 -std=c99

# c++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	@$(MKDIR_P) $(dir $@)
	@echo $(CXX) $<
	$(CXX) -c $< -o $@ -Wl,-Ttext=0x8CE00000 $(CPPFLAGS)
	
# convert textures
# $(BUILD_DIR)/romdisk/$(INPUT_TEXTURE_DIR)/%.tex: $(INPUT_TEXTURE_DIR)/%.png
# 	@$(MKDIR_P) $(dir $@)
# 	texconv --in $< --out $@ --format ARGB4444 -p -m
# $(BUILD_DIR)/romdisk/$(INPUT_TEXTURE_DIR)/%.tex: $(INPUT_TEXTURE_DIR)/%.jpg
# 	@$(MKDIR_P) $(dir $@)
# 	texconv --in $< --out $@ --format ARGB4444 -p -m

# romdisk rules
$(ROMDISK_IMAGE): $(ROMDISK_FILES)
	$(KOS_GENROMFS) -f $@ -d romdisk -v

$(ROMDISK_OBJECT): $(ROMDISK_IMAGE)
	@echo "Generated romdisk"
	$(BIN2O_EXEC) $< romdisk $@

.PHONY: bba serial emu-elf emu-cdi
bba: $(TARGET_ELF)
	dc-tool-ip -t 172.16.0.99 -x $<

serial: $(TARGET_ELF)
	sudo dc-tool-ser -b 115200 -x $< -g -c $(ISO_FOLDER)

emu-elf: $(TARGET_ELF)
	lxdream-nitro -e $< -C $(ISO_FOLDER)/

emu-cdi: $(TARGET_CDI)
	lxdream-nitro $<


.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
