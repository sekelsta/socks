CXX = g++
# For debugging use -Og -g
# For going fast use -O3
CXXFLAGS = -std=c++14 -Wall -D_GLIBCXX_DEBUG -D_LIBCXX_DEBUG_PEDANTIC -Og -g
#CXXFLAGS = -std=c++14 -Wall -O3
INCLUDE_FLAGS = -I/usr/include/libnoise -L/usr/lib 
LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lnoise -lpthread

# what folders
SRCDIR = src/
OBJDIR = obj/
BINDIR = ./

# File names
SOURCEFILES := $(notdir $(wildcard $(SRCDIR)*.cc))
SOURCES = $(addprefix $(SRCDIR), $(SOURCEFILES))
OBJECTS = $(addprefix $(OBJDIR), $(SOURCEFILES:.cc=.o))
EXEC = burrowbun

DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(INCLUDE_FLAGS) -c
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

all: main

main: main.cc utils.hh filereader.hh filereader.cc jsoninfo.hh
	$(CXX) $(CXXFLAGS) main.cc filereader.cc -o $@


# To remove generated files
clean: 
	rm -f *.o *.gch client server

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

.PHONY: all clean

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SOURCEFILES))))
