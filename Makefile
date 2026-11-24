.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "DEVKITPRO is not set. Install devkitPro and the 3DS development tools.")
endif

export DEVKITARM ?= $(DEVKITPRO)/devkitARM
export CTRULIB   ?= $(DEVKITPRO)/libctru

TARGET    := 3dsblox
BUILD     := build
SOURCES   := source
DATA      :=
INCLUDES  := include

ARCH      := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft
CFLAGS    := -g -Wall -Wextra -O2 -mword-relocations -ffunction-sections $(ARCH) $(foreach dir,$(INCLUDES),-I$(dir)) -D__3DS__
CXXFLAGS  := $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++17
ASFLAGS   := -g $(ARCH)
LDFLAGS   := -specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map) -Wl,--gc-sections
LIBPATHS  := -L$(CTRULIB)/lib
LIBS      := -lctru -lm

#---------------------------------------------------------------------------------
# no changes below this line should be needed
#---------------------------------------------------------------------------------
ifneq ($(wildcard $(DEVKITARM)/3ds_rules),)
include $(DEVKITARM)/3ds_rules
else ifneq ($(wildcard $(DEVKITPRO)/devkitarm-rules/3ds_rules),)
include $(DEVKITPRO)/devkitarm-rules/3ds_rules
else
include $(DEVKITARM)/base_rules
endif
