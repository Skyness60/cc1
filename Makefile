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


cc1: $(DEPS)
	@echo "$(BLUE_BOLD)🔨 Building $(PROJECT_NAME)...$(RESET)"
	@docker build -t $(DOCKER_IMG) . --quiet >/dev/null
	@docker run --rm -v "$(PWD)":/host $(DOCKER_IMG) cp /cc1 /host/cc1
	@echo "$(GREEN_BOLD)✅ Build complete!$(RESET)"

# Build local (sans Docker)
cc1-local:
	@echo "$(BLUE_BOLD)🔨 Building $(PROJECT_NAME) locally (cargo)...$(RESET)"
	@cargo build --release
	@ln -sf target/release/cc1 cc1
	@echo "$(GREEN_BOLD)✅ Local build complete!$(RESET)"

clean:
	@echo "$(RED_BOLD)🧹 Cleaning up...$(RESET)"
	@docker rmi $(DOCKER_IMG) >/dev/null 2>&1 || true
	@rm -f cc1
	@echo "$(GREEN_BOLD)✅ Cleanup complete!$(RESET)"

up:
	docker build -t $(DOCKER_IMG) .
	docker run --rm -v "$(PWD)":/host $(DOCKER_IMG) cp /cc1 /host/cc1


re: clean cc1

# Tests lexer: valides doivent passer, invalides doivent échouer
.PHONY: test-lex
test-lex: cc1-local
	@echo "$(PURPLE_BOLD)▶ Running lexer tests...$(RESET)"
	@fails=0; for f in tests/lex/valid_*.c; do \
		./cc1 "$$f" >/dev/null || { echo "$(RED_BOLD)ECHEC (valid): $$f$(RESET)"; fails=1; }; \
	done; \
	for f in tests/lex/invalid_*.c tests/lex/non_c89.c tests/lex/invalid_comments_*.c; do \
		./cc1 "$$f" >/dev/null && { echo "$(RED_BOLD)PASS INATTENDU (invalid): $$f$(RESET)"; fails=1; }; \
	done; \
	if [ $$fails -eq 0 ]; then echo "$(GREEN_BOLD)✅ Lexer tests OK$(RESET)"; else echo "$(RED_BOLD)❌ Lexer tests FAILED$(RESET)"; exit 1; fi

.PHONY: cc1 clean
