CC=g++
CXX_FLAGS= -Wall -Werror -O3 -I./QuickTestCPP -I./src
LD_FLAGS=

BIN_DIR=./bin
SRC_DIR=./src
TEST_DIR=./test
X86_BUILD_DIR=./.build-x86

SRC_FILES=$(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES=$(patsubst $(SRC_DIR)/%.cpp,$(X86_BUILD_DIR)/%.o,$(SRC_FILES))

APP_OBJ_FILES=$(filter-out $(X86_BUILD_DIR)/test.o, $(OBJ_FILES))
TEST_OBJ_FILES=$(filter-out $(X86_BUILD_DIR)/main.o, $(OBJ_FILES))

all: $(BIN_DIR)/unpacker $(BIN_DIR)/test

$(BIN_DIR)/unpacker: $(X86_BUILD_DIR) $(BIN_DIR) $(APP_OBJ_FILES)
	$(CC) $(APP_OBJ_FILES) $(LD_FLAGS) -o $@

$(BIN_DIR)/test: $(X86_BUILD_DIR) $(BIN_DIR) $(TEST_OBJ_FILES)
	$(CC) $(TEST_OBJ_FILES) $(LD_FLAGS) -o $@

$(BIN_DIR):
	mkdir $@

$(X86_BUILD_DIR):
	mkdir $@

$(X86_BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CXX_FLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR) $(X86_BUILD_DIR)