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
	@docker run --rm -v "$(PWD)":/host $(DOCKER_IMG) cp /usr/local/bin/cc1 /host/cc1
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
	docker run --rm -v "$(PWD)":/host $(DOCKER_IMG) cp /usr/local/bin/cc1 /host/cc1


re: clean cc1

# Tests lexer: valides doivent passer, invalides doivent échouer
.PHONY: test-lex
test-lex: cc1-local
	@echo "$(PURPLE_BOLD)▶ Running lexer tests...$(RESET)"
	@fails=0; for f in tests/lex/valid_*.c; do \
		./cc1 --lex-only "$$f" >/dev/null || { echo "$(RED_BOLD)ECHEC (valid): $$f$(RESET)"; fails=1; }; \
	done; \
	for f in tests/lex/invalid_*.c tests/lex/non_c89.c tests/lex/invalid_comments_*.c; do \
		./cc1 --lex-only "$$f" >/dev/null && { echo "$(RED_BOLD)PASS INATTENDU (invalid): $$f$(RESET)"; fails=1; }; \
	done; \
	if [ $$fails -eq 0 ]; then echo "$(GREEN_BOLD)✅ Lexer tests OK$(RESET)"; else echo "$(RED_BOLD)❌ Lexer tests FAILED$(RESET)"; exit 1; fi

# Tests parseur: compile et parse des TU valides/invalides (après préprocessing)
.PHONY: test-parse
test-parse: cc1-local
	@echo "$(PURPLE_BOLD)▶ Running parser tests...$(RESET)"
	@fails=0; \
	for f in tests/parse/valid_*.c; do \
		cppout=$$(mktemp); \
		clang -E -P -std=c89 "$$f" -o "$$cppout" || { echo "$(RED_BOLD)CPP FAIL: $$f$(RESET)"; fails=1; continue; }; \
		./cc1 --parse-tu "$$cppout" >/dev/null || { echo "$(RED_BOLD)ECHEC (valid): $$f$(RESET)"; fails=1; }; \
		rm -f "$$cppout"; \
	done; \
	for f in tests/parse/invalid_*.c; do \
		cppout=$$(mktemp); \
		clang -E -P -std=c89 "$$f" -o "$$cppout" || true; \
		./cc1 --parse-tu "$$cppout" >/dev/null && { echo "$(RED_BOLD)PASS INATTENDU (invalid): $$f$(RESET)"; fails=1; }; \
		rm -f "$$cppout"; \
	done; \
	if [ $$fails -eq 0 ]; then echo "$(GREEN_BOLD)✅ Parser tests OK$(RESET)"; else echo "$(RED_BOLD)❌ Parser tests FAILED$(RESET)"; exit 1; fi

# Tests sémantique: évalue const-expr etc. (après préprocessing)
.PHONY: test-sem
test-sem: cc1-local
	@echo "$(PURPLE_BOLD)▶ Running semantic tests...$(RESET)"
	@fails=0; \
	for f in tests/parse/valid_*.c; do \
		cppout=$$(mktemp); \
		clang -E -P -std=c89 -I tools/include "$$f" -o "$$cppout" || { echo "$(RED_BOLD)CPP FAIL: $$f$(RESET)"; fails=1; continue; }; \
		./cc1 --sem "$$cppout" >/dev/null || { echo "$(RED_BOLD)ECHEC SEM (valid): $$f$(RESET)"; fails=1; }; \
		rm -f "$$cppout"; \
	done; \
	if [ $$fails -eq 0 ]; then echo "$(GREEN_BOLD)✅ Semantic tests OK (smoke)$(RESET)"; else echo "$(RED_BOLD)❌ Semantic tests FAILED$(RESET)"; exit 1; fi

# Tests sémantique étendus (appels/varargs)
.PHONY: test-sem-calls
test-sem-calls: cc1-local
	@echo "$(PURPLE_BOLD)▶ Running semantic call/varargs tests...$(RESET)"
	@fails=0; \
	for f in tests/sem/valid_*.c; do \
		cppout=$$(mktemp); \
		clang -E -P -std=c89 -I tools/include "$$f" -o "$$cppout" || { echo "$(RED_BOLD)CPP FAIL: $$f$(RESET)"; fails=1; continue; }; \
		./cc1 --sem "$$cppout" >/dev/null || { echo "$(RED_BOLD)ECHEC SEM (valid): $$f$(RESET)"; fails=1; }; \
		rm -f "$$cppout"; \
	done; \
	for f in tests/sem/invalid_*.c; do \
		cppout=$$(mktemp); \
		clang -E -P -std=c89 -I tools/include "$$f" -o "$$cppout" || true; \
		./cc1 --sem "$$cppout" >/dev/null && { echo "$(RED_BOLD)PASS INATTENDU (invalid): $$f$(RESET)"; fails=1; }; \
		rm -f "$$cppout"; \
	done; \
	if [ $$fails -eq 0 ]; then echo "$(GREEN_BOLD)✅ Semantic call/varargs tests OK$(RESET)"; else echo "$(RED_BOLD)❌ Semantic call/varargs tests FAILED$(RESET)"; exit 1; fi

# Tests ABI/runtime (System V i386): passage/retour de structs, padding/alignment, promotions varargs
.PHONY: test-abi
test-abi: cc1-local
	@echo "$(PURPLE_BOLD)▶ Running ABI runtime tests...$(RESET)"
	@fails=0; \
	test -x ./fcc || chmod +x ./fcc; \
	run() { file="$$1"; expect="$$2"; exe="$$3"; \
		./fcc "$$file" -o "$$exe" >/dev/null 2>&1 || { echo "$(RED_BOLD)COMPILE FAIL: $$file$(RESET)"; fails=1; return; }; \
		out=$$(./"$$exe"); \
		if [ "$$out" = "$$expect" ]; then :; else echo "$(RED_BOLD)MISMATCH: $$file → got='$$out' expect='$$expect'$(RESET)"; fails=1; fi; \
		rm -f "$$exe"; \
	}; \
	run tests/abi/struct_pass_return.c "31 72 14" abi_struct; \
	run tests/abi/alignment_padding.c "1065 2066 3135" abi_align; \
	run tests/abi/varargs_promotions_runtime.c "10 30 10 11" abi_varargs; \
	if [ $$fails -eq 0 ]; then echo "$(GREEN_BOLD)✅ ABI runtime tests OK$(RESET)"; else echo "$(RED_BOLD)❌ ABI runtime tests FAILED$(RESET)"; exit 1; fi

.PHONY: cc1 clean
