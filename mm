# Environment
ENV_FILES    = 
#Compiler and Linker
CC          := g++

#The Target Binary Program
TARGET      := app

#The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR      := src
LIBDIR      := lib
INCDIR      := include
BUILDDIR    := build
TARGETDIR   := build
RESDIR      := resources
SRCEXT      := cpp
DEPEXT      := d
OBJEXT      := o

#Flags, Libraries and Includes
CFLAGS      := -fopenmp -g -Wall -Wno-unused-variable -Wno-sign-compare
LIB         := -lm -lSDL3 -lSDL3_mixer -lSDL3_ttf
INC         := -I$(INCDIR) -I$(LIBDIR)
INCDEP      := -I$(INCDIR)

#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
SOURCES     := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
SOURCES     := $(filter-out $(wildcard src/solutions/*), $(SOURCES))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

include $(ENV_FILES)
export

#Default Make
all:
	cmake --build $(TARGETDIR)

run:
	cmake --build $(TARGETDIR)
	clear
	cd $(TARGETDIR) && ./$(TARGET)

run_only:
	cd $(TARGETDIR) && ./$(TARGET)

clean:
	rm -fr $(BUILDDIR)
