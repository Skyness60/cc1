CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g -fno-stack-protector -Iinclude -MMD -MP

SRC_DIR = src
OBJ_DIR = obj
TEST_DIR = tests
TARGET = cc1
TEST_TARGET = run_tests
SRCS = $(shell find $(SRC_DIR) -name '*.cpp')
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

# Test sources (excluding main.cpp from OBJS)
MAIN_OBJ = $(OBJ_DIR)/main.o
TEST_OBJS = $(filter-out $(MAIN_OBJ),$(OBJS))

# Colors
GREEN = \033[1;32m
CYAN = \033[1;36m
YELLOW = \033[1;33m
RED = \033[1;31m
RESET = \033[0m

.PHONY: all clean re test
all: $(TARGET)
	@echo -e "$(GREEN)✓ Build successful!$(RESET)"

$(TARGET): $(OBJS)
	@echo -e "$(CYAN)[LINK]$(RESET) Linking objects into $(TARGET)"
	@$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@echo -e "$(YELLOW)[CC]$(RESET) Compiling $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Test target
test: $(TEST_OBJS) $(TEST_DIR)/test_lexer.cpp
	@echo -e "$(CYAN)[TEST]$(RESET) Building tests..."
	@$(CXX) $(CXXFLAGS) -I$(TEST_DIR) -o $(TEST_TARGET) $(TEST_DIR)/test_lexer.cpp $(TEST_OBJS)
	@echo -e "$(CYAN)[TEST]$(RESET) Running tests...\n"
	@./$(TEST_TARGET)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

clean:
	@echo -e "$(RED)[CLEAN]$(RESET) Removing $(OBJ_DIR) and $(TARGET)"
	@rm -rf $(OBJ_DIR) $(TARGET) $(TEST_TARGET)

re: clean all

-include $(DEPS)