# Directory structure
OBJECT_DIR = ./build/Debug
TARGET_DIR = ./build

# Target name
LINK_TARGET = $(TARGET_DIR)/MainHeatEquation1d

# Macro with object files
_OBJS =  \
 matrix.o \
 finiteElement.o \
 MainHeatEquation1d.o \

OBJS = $(patsubst %,$(OBJECT_DIR)/%,$(_OBJS))

# Target Build
all : $(LINK_TARGET)

# Linker command
$(LINK_TARGET) : $(OBJS)
	gcc -g -o $@ $^

# Create the directory structure if it doesn't exist
$(OBJECT_DIR):
	mkdir -p $(OBJECT_DIR)

# Compiler command
$(OBJECT_DIR)/%.o : %.c | $(OBJECT_DIR)
	gcc -g -o $@ -c $<

# Dependency Rules
$(OBJECT_DIR)/MainHeatEquation1d.o : MainHeatEquation1d.c matrix.h finiteElement.h
$(OBJECT_DIR)/finiteElement.o : finiteElement.c finiteElement.h
$(OBJECT_DIR)/matrix.o : matrix.c matrix.h

# Cleaning files
REBUILDABLES = $(OBJS) $(LINK_TARGET)

clean :
	rm -f $(REBUILDABLES)
