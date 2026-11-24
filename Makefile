#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules

#---------------------------------------------------------------------------------
TARGET      := 3dsblox
BUILD       := build
SOURCES     := source
DATA        :=
INCLUDES    := include

#---------------------------------------------------------------------------------
ARCH        := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

CFLAGS      := -g -Wall -O2 -mword-relocations \
               -fomit-frame-pointer -ffunction-sections \
               $(ARCH)

CFLAGS      += $(INCLUDE) -D__3DS__

CXXFLAGS    := $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++17

ASFLAGS     := -g $(ARCH)
LDFLAGS     =  -specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

LIBS        := -lcitro2d -lcitro3d -lctru -lm

#---------------------------------------------------------------------------------
LIBDIRS     := $(CTRULIB)

#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT   := $(CURDIR)/$(TARGET)
export TOPDIR   := $(CURDIR)

export VPATH    := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
                   $(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR  := $(CURDIR)/$(BUILD)

CFILES      := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES    := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES      := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES    := $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD := $(CC)
else
	export LD := $(CXX)
endif
#---------------------------------------------------------------------------------

export OFILES_SOURCES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES_BIN     := $(addsuffix .o,$(BINFILES))
export OFILES         := $(OFILES_BIN) $(OFILES_SOURCES)

export HFILES_BIN     := $(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE        := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
                         $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                         -I$(CURDIR)/$(BUILD)

export LIBPATHS       := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

ifeq ($(strip $(CONFIG_3D)),)
	export _3DSXDEPS := $(if $(NO_SMDH),,$(OUTPUT).smdh)
endif

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.png)
	ifneq (,$(findstring $(TARGET).png,$(icons)))
		export APP_ICON := $(CURDIR)/$(TARGET).png
	endif
endif

ifeq ($(strip $(NO_SMDH)),)
	export _3DSXFLAGS += --smdh=$(CURDIR)/$(OUTPUT).smdh
endif

.PHONY: all clean

#---------------------------------------------------------------------------------
all: $(BUILD) $(DEPSDIR)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

$(BUILD):
	@mkdir -p $@

$(DEPSDIR):
	@mkdir -p $@

$(OUTPUT).3dsx : $(OUTPUT).elf $(_3DSXDEPS)

$(OUTPUT).elf : $(OFILES)

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).3dsx $(OUTPUT).smdh $(OUTPUT).elf $(OUTPUT).map

#---------------------------------------------------------------------------------
else

DEPENDS := $(OFILES:.o=.d)

#---------------------------------------------------------------------------------
$(OUTPUT).elf : $(OFILES)

#---------------------------------------------------------------------------------
%.bin.o %_bin.h : %.bin
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
