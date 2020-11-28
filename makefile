include makefile.config

# Set variables
EXE=donut
INC_DIR=inc
SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin
USR_DIR=/usr/local/bin/

# List source files
SRC=$(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c)
OBJ=$(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
OUT=$(wildcard $(BIN_DIR)/*)

#Target - Actual Program
all: $(EXE)
	@mv $(EXE) $(BIN_DIR)/
	@echo [LD] Executable is ready!
	@echo

$(EXE): $(OBJ)
	@echo [CC] Compiled all objects
	@echo [LD] Linking object files
	@$(CC) $^ -o $@
	@echo [LD] Linked all object files


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c prep
	@mkdir -p $(@D)
	@$(CC) -I$(INC_DIR) $(CFLAGS) -c $< -o $@
	@echo [CC] Compiling $@

prep:
	@if [ -d $(OBJ_DIR) ]; then continue; else mkdir $(OBJ_DIR); fi
	@if [ -d $(BIN_DIR) ]; then continue; else mkdir $(BIN_DIR); fi

install:
	@echo [Donut] Installing Program into /usr/bin/local
	@sudo mv $(BIN_DIR)/$(EXE) $(USR_DIR)
	@echo [Donut] Installation Completed
	@echo [Donut] Use -h or --help for argument options

# Clean Action
clean:
	@rm -r $(OBJ_DIR)
	@rm -r $(BIN_DIR)
	@echo [Donut] Cleaned previous compilation
	@echo

