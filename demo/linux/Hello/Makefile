#-----------------------------------------------------------------------------
# project name:
#
PROJECT := hello

#-----------------------------------------------------------------------------
# project directories:
#

# list of all source directories used by this project
VPATH := . \

# list of all include directories needed by this project
INCLUDES := -I. \

# location of the framework (if not provided in an env. variable)
ifeq ($(Edf),)
Edf := ../../..
endif

#-----------------------------------------------------------------------------
# project files:
#

# C source files...
C_SRCS :=

# C++ source files...
CPP_SRCS := \
	Hello.cpp 

LIB_DIRS  :=
LIBS      :=


ifeq (,$(CONF))
	CONF := dbg
endif

#-----------------------------------------------------------------------------
# add C++ framework (depends on the OS this Makefile runs on):
#
ifeq ($(OS),Windows_NT)

EDF_PORT_DIR := $(Edf)/source/port/windows
#EDF_PORT_DIR := $(Edf)/ports/win32
LIB_DIRS += -L$(EDF_PORT_DIR)/$(CONF)
LIBS     += -lws2_32

else


EDF_PORT_DIR := $(Edf)/source/port/posix

CPP_SRCS += \
	Active.cpp \
	Event.cpp \
	Publish.cpp \
	Timer.cpp \
	osal.cpp


LIBS += -lpthread -lrt

endif

#============================================================================

VPATH    += $(Edf)/source $(EDF_PORT_DIR)
INCLUDES += -I$(Edf)/include -I$(Edf)/source -I$(EDF_PORT_DIR)

#-----------------------------------------------------------------------------

CC    := gcc
CPP   := g++
LINK  := g++   # for C++ programs

#-----------------------------------------------------------------------------
# basic utilities (depends on the OS this Makefile runs on):
#
ifeq ($(OS),Windows_NT)
	MKDIR      := mkdir
	RM         := rm
	TARGET_EXT := .exe
else ifeq ($(OSTYPE),cygwin)
	MKDIR      := mkdir -p
	RM         := rm -f
	TARGET_EXT := .exe
else
	MKDIR      := mkdir -p
	RM         := rm -f
	TARGET_EXT :=
endif

#-----------------------------------------------------------------------------
# build configurations...

ifeq (rel, $(CONF)) # Release configuration ..................................

BIN_DIR := build_rel
# gcc options:
CFLAGS  = -c -O3 -fno-pie -std=c99 -pedantic -Wall -Wextra -W -fpermissive\
	$(INCLUDES) $(DEFINES) -DNDEBUG

CPPFLAGS = -c -O3 -fno-pie -std=c++11 -pedantic -Wall -Wextra \
	-fno-rtti -fno-exceptions -fpermissive \
	$(INCLUDES) $(DEFINES) -DNDEBUG

else # default Debug configuration .........................................

BIN_DIR := build

# gcc options:
CFLAGS  = -c -g -O -fno-pie -std=c99 -pedantic -Wall -Wextra -W -fpermissive\
	$(INCLUDES) $(DEFINES)

CPPFLAGS = -c -g -O -fno-pie -std=c++11 -pedantic -Wall -Wextra \
	-fno-rtti -fno-exceptions -fpermissive\
	$(INCLUDES) $(DEFINES)

endif  # .....................................................................

ifndef GCC_OLD
	LINKFLAGS := -no-pie
endif

#-----------------------------------------------------------------------------
C_OBJS       := $(patsubst %.c,%.o,   $(C_SRCS))
CPP_OBJS     := $(patsubst %.cpp,%.o, $(CPP_SRCS))

TARGET_EXE   := $(BIN_DIR)/$(PROJECT)$(TARGET_EXT)
C_OBJS_EXT   := $(addprefix $(BIN_DIR)/, $(C_OBJS))
C_DEPS_EXT   := $(patsubst %.o,%.d, $(C_OBJS_EXT))
CPP_OBJS_EXT := $(addprefix $(BIN_DIR)/, $(CPP_OBJS))
CPP_DEPS_EXT := $(patsubst %.o,%.d, $(CPP_OBJS_EXT))

# create $(BIN_DIR) if it does not exist
ifeq ("$(wildcard $(BIN_DIR))","")
$(shell $(MKDIR) $(BIN_DIR))
endif

#-----------------------------------------------------------------------------
# rules
#

all: $(TARGET_EXE)

$(TARGET_EXE) : $(C_OBJS_EXT) $(CPP_OBJS_EXT)
	$(LINK) $(LINKFLAGS) $(LIB_DIRS) -o $@ $^ $(LIBS)

$(BIN_DIR)/%.d : %.c
	$(CC) -MM -MT $(@:.d=.o) $(CFLAGS) $< > $@

$(BIN_DIR)/%.d : %.cpp
	$(CPP) -MM -MT $(@:.d=.o) $(CPPFLAGS) $< > $@

$(BIN_DIR)/%.o : %.c
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/%.o : %.cpp
	$(CPP) $(CPPFLAGS) $< -o $@

.PHONY : clean show

# include dependency files only if our goal depends on their existence
ifneq ($(MAKECMDGOALS),clean)
  ifneq ($(MAKECMDGOALS),show)
-include $(C_DEPS_EXT) $(CPP_DEPS_EXT)
  endif
endif

.PHONY : clean show

clean :
	-$(RM) $(BIN_DIR)/*.o \
	$(BIN_DIR)/*.d \
	$(TARGET_EXE)

show :
	@echo PROJECT      = $(PROJECT)
	@echo TARGET_EXE   = $(TARGET_EXE)
	@echo VPATH        = $(VPATH)
	@echo C_SRCS       = $(C_SRCS)
	@echo CPP_SRCS     = $(CPP_SRCS)
	@echo C_DEPS_EXT   = $(C_DEPS_EXT)
	@echo C_OBJS_EXT   = $(C_OBJS_EXT)
	@echo C_DEPS_EXT   = $(C_DEPS_EXT)
	@echo CPP_DEPS_EXT = $(CPP_DEPS_EXT)
	@echo CPP_OBJS_EXT = $(CPP_OBJS_EXT)
	@echo LIB_DIRS     = $(LIB_DIRS)
	@echo LIBS         = $(LIBS)
	@echo DEFINES      = $(DEFINES)

