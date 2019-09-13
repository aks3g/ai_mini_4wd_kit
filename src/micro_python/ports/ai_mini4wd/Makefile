include ../../py/mkenv.mk

AUP_BUILDER = ../../../tools/build_aup.py
RM = rm -f
MV = mv -f

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

FATFS_DIR=lib/oofatfs

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I../../../library/include/
INC += -I$(PACKS_PATH)/atmel/SAMD51_DFP/1.0.70/include/
INC += -I$(PACKS_PATH)/arm/CMSIS/Include/


LD = $(CROSS_COMPILE)gcc

DFU = $(TOP)/tools/dfu.py
PYDFU = $(TOP)/tools/pydfu.py
#CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mabi=aapcs-linux -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
CFLAGS_CORTEX_M4 = -mthumb -D__SAMD51J20A__ -mcpu=cortex-m4
CFLAGS = -x c $(INC) -D__SAMD51J20A__ -Wall -Werror -std=c99 -nostdlib $(CFLAGS_MOD) $(CFLAGS_EXTRA) $(CFLAGS_CORTEX_M4) $(COPT)
LDFLAGS = -mthumb -Wl,-Map=$@.map  -Wl,--start-group -lm  -Wl,--end-group -Wl,--gc-sections -mcpu=cortex-m4 -T samd51j20a_flash.ld
LDFLAGS_BOOTLOADER = -mthumb -Wl,-Map=$@.map  -Wl,--start-group -lm  -Wl,--end-group -Wl,--gc-sections -mcpu=cortex-m4 -T samd51j20a_bootloader.ld

# Tune for Debugging or Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -DDEBUG
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif


LIB = -L../../../library/lib
LIBS = -laimini4wd -lsamd51 -lm

SRC_S = \
	gchelper.s

SRC_C = \
	lib/oofatfs/ff.c \
	lib/oofatfs/option/unicode.c \
	main.c \
	mini4wd.c \
	mini4wd_type.c \
	self_position_estimater_type.c \
	machine_learning_type.c \
	storage_if.c \
	modpyb.c \
	startup_samd51.c \
	system_samd51.c \
	syscalls.c \
	lib/timeutils/timeutils.c \
	lib/utils/printf.c \
	lib/utils/stdout_helpers.c \
	lib/utils/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	$(BUILD)/_frozen_mpy.c \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o)) $(addprefix $(BUILD)/, $(SRC_S:.s=.o))

$(BUILD)/$(FATFS_DIR)/ff.o: COPT += -Os
$(filter $(PY_BUILD)/../extmod/vfs_fat_%.o, $(PY_O)): COPT += -Os

all: $(BUILD)/MINI4WD.AUP $(BUILD)/firmware.bin

$(BUILD)/_frozen_mpy.c: frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)$(TOP)/tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=none $< > $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) $(LIB) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/MINI4WD.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS_BOOTLOADER) $(LIB) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary -j .isr_vector -j .text -j .relocate $^ $(BUILD)/firmware.bin

$(BUILD)/MINI4WD.bin: $(BUILD)/MINI4WD.elf
	$(Q)$(OBJCOPY) -O binary -j .isr_vector -j .text -j .relocate $^ $(BUILD)/MINI4WD.bin

$(BUILD)/MINI4WD.AUP: $(BUILD)/MINI4WD.bin
	$(ECHO) "Create $@"
	$(Q)$(RM) MINI4WD.AUP
	$(Q)$(PYTHON) $(AUP_BUILDER) $(BUILD)/MINI4WD.bin
	$(Q)$(MV) MINI4WD.AUP $@

include $(TOP)/py/mkrules.mk
