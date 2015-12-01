COMPILER = g++
#COMPILER = clang++
#COMPILER = g++-4.4
CFLAGS    =
CFLAGS   += -Wall -Wextra -Winit-self -Wno-unused-parameter -Wfloat-equal
CFLAGS   += -std=c++11
CFLAGS   += -O3
#CFLAGS   += -g
CFLAGS   += `root-config --cflags`
LDFLAGS   =
LDFLAGS  += `root-config --libs`
LIBS      = `MyLibrary-config`
INCLUDE   = -I../include
INSTALL_DIR = ~/bin

TARGET   = rfv

SOURCES  = $(wildcard *.cc)
OBJECTS  = $(SOURCES:.cc=.o)

$(TARGET): $(OBJECTS)
	$(COMPILER) -o $@ $^ $(LDFLAGS) $(LIBS)

%.o: %.cc ../include/misc.hh
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $< $(LIBS)

# Argument.o: Drawer.cc ../include/misc.hh

all: clean $(TARGET)

install: $(TARGET)
	install -s $(TARGET) $(INSTALL_DIR)

clean:
	rm -f $(OBJECTS) $(TARGET) *~ \#* ../.~ ../include/*~ ../include/\#*
