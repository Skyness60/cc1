PROJECT_NAME := cc1
DOCKER_IMG := cc1

SOURCES := $(shell find src -type f -name "*.rs" 2>/dev/null || echo "src/main.rs")
DEPS := Dockerfile Cargo.toml Cargo.lock $(SOURCES)

cc1: $(DEPS)
	docker build -t $(DOCKER_IMG) .
	docker run --rm -v "$(PWD)":/host $(DOCKER_IMG) cp /cc1 /host/cc1

clean:
	docker rmi $(DOCKER_IMG) || true
	rm -f cc1

re: clean cc1

.PHONY: cc1 clean
