CC         := mpicc
CFLAGS     := -Wall -Wextra -Wpedantic -std=c2x -g -O2 -Iinclude
LDFLAGS    := 
PROC_REQ   := 5

# --- DIRECTORIOS ---
SRC_DIR    := src
TEST_DIR   := tests
OBJ_DIR    := build
BIN_DIR    := bin

# --- OBJETOS ---
OBJS       := $(OBJ_DIR)/main.o \
              $(OBJ_DIR)/coordinator.o \
              $(OBJ_DIR)/lane.o \
              $(OBJ_DIR)/vehicle.o \
              $(OBJ_DIR)/wrapper_mpi.o \

# --- EJECUTABLE FINAL ---
TARGET     := $(BIN_DIR)/interseccion_mpi

# --- REGLAS PRINCIPALES ---
all: $(TARGET)

# Linking del ejectable final
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $^ $(LDFLAGS) -o $@
	@echo "Compilation completed on $@. Me go home. :3"

# Compilación
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Creación de carpetas
$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

# --- EJECUCIÓN ---
run:
	mpirun -np $(PROC_REQ) --oversubscribe $(TARGET)

# --- LIMPIEZA ---
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Clean completed. Me go home."

.PHONY: all run clean