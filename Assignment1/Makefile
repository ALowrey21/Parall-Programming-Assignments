CC := gcc
OPT ?= -O2
CFLAGS := -std=c11 $(OPT)
CPPFLAGS := -D_POSIX_C_SOURCE=200809L -MMD -MP
LDLIBS := -lm

SRC_DIR  := src
OBJ_DIR  := build
DATA_DIR := data




PROGRAMS := ArrayMaximum MatrixMultiplication ExclusivePrefixSum
COMMON := lowreytiming plot
COMMON_OBJS := $(COMMON:%=$(OBJ_DIR)/%.o)
ALL_OBJS := $(PROGRAMS:%=$(OBJ_DIR)/%.o) $(COMMON_OBJS)

.PHONY: all clean
all: $(PROGRAMS) | $(DATA_DIR)

#These programs are built from build/%.o plus helpers
$(PROGRAMS): %: $(OBJ_DIR)/%.o $(COMMON_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# Create folders when they are missing
$(OBJ_DIR) $(DATA_DIR):
	mkdir -p $@

#Allows cleaning up. Removes the post compiled version of the c files
clean:
	rm -rf $(OBJ_DIR) $(PROGRAMS)

-include $(ALL_OBJS:.o=.d)