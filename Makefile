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
BUILDDIR    := obj
TARGETDIR   := bin
RESDIR      := resources
SRCEXT      := cpp
DEPEXT      := d
OBJEXT      := o

#Flags, Libraries and Includes
CFLAGS      := -fopenmp -g -Wall -Wno-unused-variable -Wno-sign-compare
LIB         := -lm -lSDL3 -lfreetype
INC         := -I$(INCDIR) -I$(LIBDIR) -I/usr/include/freetype2 -I/usr/include/libpng16
INCDEP      := -I$(INCDIR) -I/usr/include/freetype2 -I/usr/include/libpng16

#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
SOURCES     := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
SOURCES     := $(filter-out $(wildcard src/solutions/*), $(SOURCES))
SOURCES     := $(filter-out $(wildcard src/maze.cpp), $(SOURCES))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

include $(ENV_FILES)
export

#Default Make
all: resources $(TARGET)

run: resources $(TARGET)
	cd $(TARGETDIR) && LD_PRELOAD=$(shell pwd)/build/libSDL3.so.0 ./$(TARGET)
	# cd $(TARGETDIR) && ./$(TARGET)

run_only:
	cd $(TARGETDIR) && LD_PRELOAD=$(shell pwd)/build/libSDL3.so.0 ./$(TARGET)
	# cd $(TARGETDIR) && ./$(TARGET)

#Remake
remake: cleaner all

#Copy Resources from Resources Directory to Target Directory
resources: directories
	-@cp -r $(RESDIR)/* $(TARGETDIR)/

#Make the Directories
directories:
	-@mkdir -p $(TARGETDIR)
	-@mkdir -p $(BUILDDIR)

#Clean only Objecst
clean:
	@$(RM) -rf $(BUILDDIR)

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf $(TARGETDIR)

#Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

#Link
$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGETDIR)/$(TARGET) $^ $(LIB) 

#Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	@cp -fr $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	@sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	@sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	@rm -fr $(BUILDDIR)/$*.$(DEPEXT).tmp

#Non-File Targets
.PHONY: all remake clean cleaner resources
