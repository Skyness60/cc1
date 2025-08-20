PROJECT_NAME := cc1
DOCKER_IMG := cc1

SOURCES := $(shell find src -type f -name "*.rs" 2>/dev/null || echo "src/main.rs")
DEPS := Dockerfile Cargo.toml $(SOURCES)

RED_BOLD := \033[1;31m
GREEN_BOLD := \033[1;32m
PURPLE_BOLD := \033[1;35m
ORANGE_BOLD := \033[1;33m
YELLOW_BOLD := \033[1;33m
BLUE_BOLD := \033[1;34m
CYAN_BOLD := \033[1;36m
WHITE_BOLD := \033[1;37m
BLACK_BOLD := \033[1;30m
RESET := \033[0m

# Default target - build cc1 in container when running 'make'
all: cc1

# Build main target - all operations in container
cc1: $(DEPS)
	@echo "$(BLUE_BOLD)🔨 Building $(PROJECT_NAME) in container...$(RESET)"
	@docker build -t $(DOCKER_IMG) . --quiet >/dev/null
	@echo "$(CYAN_BOLD)🔗 Creating local cc1 symlink...$(RESET)"
	@cargo build --release --quiet
	@ln -sf target/release/cc1 cc1
	@echo "$(GREEN_BOLD)✅ Build complete! All tools available in container + cc1 executable in root.$(RESET)"

# Alternative local build (for development)
cc1-local:
	@echo "$(BLUE_BOLD)🔨 Building $(PROJECT_NAME) locally...$(RESET)"
	@cargo build --release
	@ln -sf target/release/cc1 cc1
	@echo "$(GREEN_BOLD)✅ Local build complete!$(RESET)"

clean:
	@echo "$(RED_BOLD)🧹 Cleaning up...$(RESET)"
	@docker rmi $(DOCKER_IMG) >/dev/null 2>&1 || true
	@rm -f cc1
	@cargo clean 2>/dev/null || true
	@echo "$(GREEN_BOLD)✅ Cleanup complete!$(RESET)"

# Update container (rebuild)
up:
	@echo "$(BLUE_BOLD)🔄 Rebuilding container...$(RESET)"
	@docker build -t $(DOCKER_IMG) . --no-cache
	@echo "$(GREEN_BOLD)✅ Container rebuilt!$(RESET)"

# Test targets using containerized tools
test-compile:
	@echo "$(CYAN_BOLD)🧪 Testing compilation with containerized tools...$(RESET)"
	@./fcc tests/hello_world.c -o hello_test
	@./hello_test
	@rm -f hello_test
	@echo "$(GREEN_BOLD)✅ Compilation test passed!$(RESET)"

test-all-modes:
	@echo "$(CYAN_BOLD)🧪 Testing all compilation modes...$(RESET)"
	@./fcc tests/simple_test.c -E > /dev/null  # Preprocess only
	@./fcc tests/simple_test.c -S > /dev/null  # Compile to assembly
	@./fcc tests/simple_test.c -c > /dev/null  # Compile to object
	@./fcc tests/simple_test.c -o test_all     # Full compilation
	@./test_all
	@rm -f test_all simple_test.o simple_test.s
	@echo "$(GREEN_BOLD)✅ All modes test passed!$(RESET)"

# Container shell for debugging
shell:
	@echo "$(PURPLE_BOLD)🐚 Opening container shell...$(RESET)"
	@docker run --rm -it -v "$(PWD)":/workspace -w /workspace $(DOCKER_IMG) bash

re: clean cc1

.PHONY: cc1 cc1-local clean up test-compile test-all-modes shell re
