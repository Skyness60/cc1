CXX = g++
CXXFLAGS = -std=c++26 -Wall -Wextra -Iinclude

SRC_DIR = src
OBJ_DIR = obj
TARGET = cc1
SRCS = $(shell find $(SRC_DIR) -name '*.cpp')
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Colors
GREEN = \033[1;32m
CYAN = \033[1;36m
YELLOW = \033[1;33m
RED = \033[1;31m
RESET = \033[0m

.PHONY: all clean re
all: $(TARGET)
	@echo -e "$(GREEN)✓ Build successful!$(RESET)"

$(TARGET): $(OBJS)
	@echo -e "$(CYAN)[LINK]$(RESET) Linking objects into $(TARGET)"
	@$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo -e "$(YELLOW)[CC]$(RESET) Compiling $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@


$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@echo -e "$(RED)[CLEAN]$(RESET) Removing $(OBJ_DIR) and $(BIN_DIR)"
	@rm -rf $(OBJ_DIR) $(BIN_DIR)

re: clean all