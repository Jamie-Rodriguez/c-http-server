# Adapted from Hilton Lipschitz's makefile setup that he detailed on his blog
# https://hiltmon.com/blog/2013/07/03/a-simple-c-plus-plus-project-structure

CC := clang
# Note: Disabled -Wincompatible-pointer-types-discards-qualifiers
CFLAGS := -g -Wall -Wextra -Wpedantic -Wconversion -Wno-incompatible-pointer-types-discards-qualifiers -D_FORTIFY_SOURCE=2 -ffunction-sections -fdata-sections
SRCEXT := c

SRCDIR := src
LIBDIR := lib
BUILDDIR := build
BINDIR := bin

# I use this variable to filter out the entrypoint of the "runtime" executable
# when compiling for the "test" executable
ENTRYPOINTOBJ := server.o
TARGET := $(BINDIR)/server

TESTDIR := test
TESTTARGET := $(BINDIR)/runTests

# External Library: Unity
UNITYSOURCEDIR := Unity-2.5.2/src
UNITYSOURCES := $(shell find $(LIBDIR)/$(UNITYSOURCEDIR) -type f -name *.$(SRCEXT))
UNITYOBJECTS := $(patsubst $(LIBDIR)/$(UNITYSOURCEDIR)/%,$(BUILDDIR)/%,$(UNITYSOURCES:.$(SRCEXT)=.o))

INC := -I include -I $(LIBDIR)/$(UNITYSOURCEDIR)

SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

TESTSOURCES := $(shell find $(TESTDIR) -type f -name *.$(SRCEXT))
TESTOBJECTS := $(patsubst $(TESTDIR)/%,$(BUILDDIR)/%,$(TESTSOURCES:.$(SRCEXT)=.o))


$(TARGET): $(OBJECTS)
	@echo "Linking..."
	@mkdir -p $(BINDIR)
	$(CC) $^ -o $(TARGET)

# server.o contains the entrypoint of the non-test code, filter it out so there aren't two entrypoints
test: $(UNITYOBJECTS) $(TESTOBJECTS) $(filter-out $(BUILDDIR)/$(ENTRYPOINTOBJ),$(OBJECTS))
	@echo "Linking test object files...";
	@mkdir -p $(BINDIR)
	$(CC) $^ -o $(TESTTARGET)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo "Building object files...";
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(BUILDDIR)/%.o: $(TESTDIR)/%.$(SRCEXT)
	@echo "Building test object files...";
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(BUILDDIR)/%.o: $(LIBDIR)/$(UNITYSOURCEDIR)/%.$(SRCEXT)
	@echo "Building Unity lib object files...";
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo "Cleaning...";
	$(RM) -r $(BUILDDIR)/*.o $(TARGET) $(TESTTARGET)

.PHONY: clean
