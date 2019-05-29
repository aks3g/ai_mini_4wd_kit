include ../../py/mkenv.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I../../../library/include/
INC += -I$(PACKS_PATH)/atmel/SAMD51_DFP/1.0.70/include/
INC += -I$(PACKS_PATH)/arm/cmsis/5.0.1/CMSIS/Include/


LD = $(CROSS_COMPILE)gcc

DFU = $(TOP)/tools/dfu.py
PYDFU = $(TOP)/tools/pydfu.py
#CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mabi=aapcs-linux -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
CFLAGS_CORTEX_M4 = -mthumb -D__SAMD51J20A__ -mcpu=cortex-m4
CFLAGS = -x c $(INC) -D__SAMD51J20A__ -Wall -Werror -std=c99 -nostdlib $(CFLAGS_CORTEX_M4) $(COPT)
LDFLAGS = -mthumb -Wl,-Map=$@.map  -Wl,--start-group -lm  -Wl,--end-group -Wl,--gc-sections -mcpu=cortex-m4 -T samd51j20a_flash.ld


# Tune for Debugging or Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -DDEBUG
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif


LIB = -L../../../library/lib
LIBS = -laimini4wd -lsamd51 -lm

SRC_C = \
	main.c \
	mini4wd.c \
	mini4wd_type.c \
	modpyb.c \
	startup_samd51.c \
	system_samd51.c \
	syscalls.c \
	lib/utils/printf.c \
	lib/utils/stdout_helpers.c \
	lib/utils/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	$(BUILD)/_frozen_mpy.c \

OBJ = $(PY_O) $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

all: $(BUILD)/firmware.dfu

$(BUILD)/_frozen_mpy.c: frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)$(TOP)/tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=none $< > $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) $(LIB) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary -j .isr_vector -j .text -j .data $^ $(BUILD)/firmware.bin

$(BUILD)/firmware.dfu: $(BUILD)/firmware.bin
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(DFU) -b 0x08000000:$(BUILD)/firmware.bin $@

deploy: $(BUILD)/firmware.dfu
	$(ECHO) "Writing $< to the board"
	$(Q)$(PYTHON) $(PYDFU) -u $<


# Run emulation build on a POSIX system with suitable terminal settings
run:
	stty raw opost -echo
	build/firmware.elf
	@echo Resetting terminal...
# This sleep is useful to spot segfaults
	sleep 1
	reset

test: $(BUILD)/firmware.elf
	$(Q)/bin/echo -e "print('hello world!', list(x+1 for x in range(10)), end='eol\\\\n')\\r\\n\\x04" | $(BUILD)/firmware.elf | tail -n2 | grep "^hello world! \\[1, 2, 3, 4, 5, 6, 7, 8, 9, 10\\]eol"

include $(TOP)/py/mkrules.mk
