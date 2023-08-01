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
INC += -I../../../mini4wd_v5/
INC += -I../../../packs/atmel/SAMD51_DFP/1.0.70/include/
INC += -I../../../packs/arm/CMSIS/Include/


LD = $(CROSS_COMPILE)gcc

DFU = $(TOP)/tools/dfu.py
PYDFU = $(TOP)/tools/pydfu.py
#CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mabi=aapcs-linux -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=soft -fsingle-precision-constant -Wdouble-promotion
CFLAGS_CORTEX_M4 = -mthumb -D__SAMD51J20A__ -mcpu=cortex-m4
CFLAGS = -x c $(INC) -D__SAMD51J20A__  -mcpu=cortex-m4 -mtune=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=softfp -Wall -Werror -std=c99 -nostdlib $(CFLAGS_MOD) $(CFLAGS_EXTRA) $(CFLAGS_CORTEX_M4) $(COPT)
LDFLAGS = -mthumb -Wl,-Map=$@.map  -Wl,--start-group -lm  -Wl,--end-group -Wl,--gc-sections -mcpu=cortex-m4 -T samd51j20a_flash.ld
LDFLAGS_BOOTLOADER = -mthumb -Wl,-Map=$@.map  -Wl,--start-group -lm  -Wl,--end-group -Wl,--gc-sections -mcpu=cortex-m4 -T samd51j20a_bootloader.ld
ARFLAGS_LIB = 

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
	$(BUILD)/_frozen_mpy.c

SRC_LIB_C = \
	wrapper.c \
	mini4wd.c \
	mini4wd_type.c \
	self_position_estimater_type.c \
	machine_learning_type.c \
	vfs_impl.c \
	modpyb.c \
	lib/timeutils/timeutils.c \
	lib/utils/printf.c \
	lib/utils/stdout_helpers.c \
	lib/utils/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	$(BUILD)/_frozen_mpy.c


OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o)) $(addprefix $(BUILD)/, $(SRC_S:.s=.o))
LIB_OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_LIB_C:.c=.o)) $(addprefix $(BUILD)/, $(SRC_S:.s=.o))

$(BUILD)/$(FATFS_DIR)/ff.o: COPT += -Os
$(filter $(PY_BUILD)/../extmod/vfs_fat_%.o, $(PY_O)): COPT += -Os

all: $(BUILD)/MINI4WD.AUP
lib: $(BUILD)/libmpython_mini4wd.a

$(BUILD)/_frozen_mpy.c: frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)$(TOP)/tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=none $< > $@

$(BUILD)/MINI4WD.elf: $(OBJ)
	$(ECHO) "LINK $@"
	echo $(Q)$(LD) $(LDFLAGS_BOOTLOADER) $(LIB) -o $@ $^ $(LIBS)
	$(Q)$(LD) $(LDFLAGS_BOOTLOADER) $(LIB) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/libmpython_mini4wd.a: $(LIB_OBJ)
	$(ECHO) "ARCHIVE $@"
	$(Q)$(AR) $(ARFLAGS_LIB)-r $@ $^
	$(CP) $@ ../../../library/lib/libmpython_mini4wd.a
	$(Q)$(SIZE) $@


$(BUILD)/MINI4WD.bin: $(BUILD)/MINI4WD.elf
	$(Q)$(OBJCOPY) -O binary -j .isr_vector -j .text -j .relocate $^ $(BUILD)/MINI4WD.bin

$(BUILD)/MINI4WD.AUP: $(BUILD)/MINI4WD.bin
	$(ECHO) "Create $@"
	$(Q)$(RM) MINI4WD.AUP
	$(Q)$(PYTHON) $(AUP_BUILDER) $(BUILD)/MINI4WD.bin
	$(Q)$(MV) MINI4WD.AUP $@

include $(TOP)/py/mkrules.mk
