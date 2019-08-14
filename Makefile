export PROJECT = cli++
export VERSION = 0.1.0

NAMESPACE = nova

BUILD = .
PREFIX = $(DEVROOT)
DIST = false

LIBRARY = lib$(PROJECT).so
LIBFILE = $(LIBRARY).$(VERSION)

INCDIR = include
SRCDIR = src
OBJDIR = $(BUILD)/obj
LIBDIR = $(BUILD)/lib

TEST = @$(MAKE) -C test

CC = g++
CCFLAGS = -I $(INCDIR) -std=gnu++17 -Wall

ifeq ($(DIST),false)
	CCFLAGS += -I $(DEVROOT)/include
endif

SRC := $(wildcard $(SRCDIR)/*.cpp)
OBJ := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
TARGET = $(LIBDIR)/$(LIBFILE)
HINSTALL = $(INCDIR)/$(NAMESPACE)
HEADERS = $(wildcard $(HINSTALL)/*.h)

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) -o $@ -c -fpic $(CCFLAGS) $^

$(TARGET) : $(OBJ)
	$(CC) -o $@ -shared $(CCFLAGS) $^
	@cd $(LIBDIR) && ln -sf $(LIBFILE) $(LIBRARY)

.PHONY: all clean cleanall cleantest dir install test version

all : dir $(TARGET)

clean :
	rm -rf $(OBJDIR) $(LIBDIR)

cleanall : clean cleantest

cleantest :
	$(TEST) clean

dir :
	@mkdir -p $(OBJDIR) $(LIBDIR)

install :
	@mkdir -p $(PREFIX)/{$(HINSTALL),$(LIBDIR)}
	@echo COPY header files
	@cp -av $(HEADERS) $(PREFIX)/$(HINSTALL)
	@echo INSTALL $(PREFIX)/$(TARGET)
	@cp $(TARGET) $(PREFIX)/$(LIBDIR)
	@cd $(PREFIX)/$(LIBDIR) && ln -sf $(LIBFILE) $(LIBRARY)

export ARGS # Arguments to pass to the test program.
test :
	$(TEST) run

version :
	@echo Project: $(PROJECT)
	@echo Version: $(VERSION)
