# === Configuration ===
MCU      = atmega324pa
F_CPU    = 8000000
TARGET   = $(notdir $(CURDIR))
LIBNAME  = libm324pa-$(TARGET).a

# === Paths ===
SRC_DIR  = source
INC_DIR  = include
OBJ_DIR  = bin/obj
BIN_DIR  = bin

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
LIB_FILE  = $(BIN_DIR)/$(LIBNAME)

# === Tools ===
CC   = avr-g++
AR   = avr-ar
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -std=c++14 -I$(INC_DIR) -I$(SRC_DIR)

# === Default target ===
all: $(LIB_FILE)

# === Archive rule ===
$(LIB_FILE): $(OBJ_FILES) | $(BIN_DIR)
	$(AR) rcs $@ $^
	@echo "Built: $@"

# === Compile each source file to object file ===
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -Wl,-u,__uart0_isr_reference -Wl,-u,__uart1_isr_reference

# === Create output directories ===
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# === Clean ===
clean:
	rm -rf $(BIN_DIR)

debug:
	@echo TARGET=$(TARGET)
	@echo LIBNAME=$(LIBNAME)

.PHONY: all clean
