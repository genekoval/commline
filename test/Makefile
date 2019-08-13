BINDIR = bin

CC = g++
CCFLAGS = -std=gnu++17 -Wall \
	-D PROJECT=\"$(PROJECT)\" -D VERSION=\"$(VERSION)\" \
	-I ../include -I $(DEVROOT)/include \
	-L../lib -Wl,-rpath=../lib \
	-L$(DEVROOT)/lib -Wl,-rpath=$(DEVROOT)/lib

LDLIBS = -llauncher++

TARGET = $(BINDIR)/$(PROJECT).$(VERSION).test

$(TARGET): main.cpp
	@mkdir -p $(BINDIR)
	$(CC) -o $@ $(CCFLAGS) $< $(LDLIBS)

.PHONY: clean run

clean:
	rm -rf $(BINDIR)

run: $(TARGET)
	@$<
