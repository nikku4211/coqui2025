#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

include $(DEVKITARM)/gba_rules

#---------------------------------------------------------------------------------
# the LIBGBA path is defined in gba_rules, but we have to define LIBTONC ourselves
#---------------------------------------------------------------------------------
LIBTONC := $(DEVKITPRO)/libtonc
grit := ../tools/grit.exe
ptexconv := ../tools/ptexconv_x64.exe
py := python

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
# DATA is a list of directories containing binary data
# GRAPHICS is a list of directories containing files to be processed by grit
#
# All directories are specified relative to the project directory where
# the makefile is found
#
#---------------------------------------------------------------------------------
TARGET		:= coqui2025
BUILD		:= build
SOURCES		:= src
INCLUDES	:= include
GRAPHICS := art
LEVELS	:= level
DATA		:= data
MUSIC		:= music
SFX			:= sfx
SAMPLES := samples

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-mthumb -mthumb-interwork

CFLAGS	:=	-g -Wall -O2\
		-mcpu=arm7tdmi -mtune=arm7tdmi\
		$(ARCH)

CFLAGS	+=	$(INCLUDE)

CXXFLAGS	:=	$(CFLAGS) -fno-rtti -fno-exceptions

ASFLAGS	:=	-g $(ARCH)
LDFLAGS	=	-g $(ARCH) -Wl,-Map,$(notdir $*.map)

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:= -ltonc


#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:=	$(LIBTONC)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------


ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir)) \
			$(foreach dir,$(LEVELS),$(CURDIR)/$(dir)) \
			$(foreach dir,$(GRAPHICS),$(CURDIR)/$(dir)) \
			$(foreach dir,$(MUSIC),$(CURDIR)/$(dir)) \
			$(foreach dir,$(SFX),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c))) $(foreach dir,$(LEVELS),$(notdir $(wildcard $(dir)/*.c))) $(foreach dir,$(MUSIC),$(notdir $(wildcard $(dir)/*.c))) $(foreach dir,$(SFX),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
PNGFILES	:=	$(foreach dir,$(GRAPHICS),$(notdir $(wildcard $(dir)/*.png)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.raw)))
JSONFILES	:=	$(foreach dir,$(GRAPHICS),$(notdir $(wildcard $(dir)/*.json)))

ifneq ($(strip $(MUSIC)),)
	export AUDIOFILES	:=	$(foreach dir,$(notdir $(wildcard $(MUSIC)/*.*)),$(CURDIR)/$(MUSIC)/$(dir))
	#BINFILES += soundbank.bin
endif

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_BIN := $(addsuffix .o,$(BINFILES))

export OFILES_SOURCES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export OFILES_GRAPHICS := $(PNGFILES:.png=.o) $(JSONFILES:.json=_metasprite.o)

export OFILES := $(OFILES_BIN) $(OFILES_SOURCES) $(OFILES_GRAPHICS) $(OFILES_SAMPLES)

export HFILES := $(addsuffix .h,$(subst .,_,$(BINFILES))) $(PNGFILES:.png=.h) $(JSONFILES:.json=_metasprite.h)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-iquote $(CURDIR)/$(dir)) \
					$(foreach dir,$(LEVELS),-iquote $(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).elf $(TARGET).gba


#---------------------------------------------------------------------------------
else

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

$(OUTPUT).gba	:	$(OUTPUT).elf

$(OUTPUT).elf	:	$(OFILES)

$(OFILES_SOURCES) : $(HFILES)

#---------------------------------------------------------------------------------
# The bin2o rule should be copied and modified
# for each extension used in the data directories
#---------------------------------------------------------------------------------

#---------------------------------------------------------------------------------
# rule to build soundbank from music files
#---------------------------------------------------------------------------------
#soundbank.bin soundbank.h : $(AUDIOFILES)
#---------------------------------------------------------------------------------
#	@mmutil $^ -osoundbank.bin -hsoundbank.h

#---------------------------------------------------------------------------------
# This rule links in binary data with the .raw extension
#---------------------------------------------------------------------------------
%.raw.o	%_raw.h :	%.raw
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
# this rule converts aseprite sprite sheet jsons to c metasprites via python
#---------------------------------------------------------------------------------
%_metasprite.c %_metasprite.h &: %.json
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(py) ../tools/asejsontoc.py $< $(basename $@).c

#---------------------------------------------------------------------------------
# This rule creates assembly source files using grit
# grit takes an image file and a .grit describing how the file is to be processed
# add additional rules like this for each image extension
# you use in the graphics folders
# We will use PTexConv instead though.
#---------------------------------------------------------------------------------
testtileset.c testtileset.h &: testtileset.png
#---------------------------------------------------------------------------------
	@echo "$(ptexconv) $(notdir $<)"
	$(ptexconv) -gb -b 4 -p 1 -pb 0 -pc -ns -cn $< -oc -o $(basename $@)
	
#---------------------------------------------------------------------------------
coqmansheet.c coqmansheet.h &: coqmansheet.png
#---------------------------------------------------------------------------------
	@echo "$(ptexconv) $(notdir $<)"
	$(ptexconv) -gb -b 4 -p 1 -pb 0 -pc -ns -cn $< -oc -o $(basename $@)
	
#---------------------------------------------------------------------------------
coqtongue.c coqtongue.h &: coqtongue.png
#---------------------------------------------------------------------------------
	@echo "$(ptexconv) $(notdir $<)"
	$(ptexconv) -gb -b 4 -p 1 -pb 0 -pc -ns -cn $< -oc -o $(basename $@)

#---------------------------------------------------------------------------------
prulersheet.c prulersheet.h &: prulersheet.png
#---------------------------------------------------------------------------------
	@echo "$(ptexconv) $(notdir $<)"
	$(ptexconv) -gb -b 4 -p 1 -pb 0 -pc -ns -cn $< -oc -o $(basename $@)
	
# make likes to delete intermediate files. This prevents it from deleting the
# files generated by grit after building the GBA ROM.
.SECONDARY:

-include $(DEPSDIR)/*.d
#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
