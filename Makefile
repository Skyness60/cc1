PROJECT_NAME := cc1
DOCKER_IMG := cc1

# Build and extract the cc1 executable in one command
all:
	docker build -t $(DOCKER_IMG) .
	docker run --rm -v "$(PWD)":/host $(DOCKER_IMG) cp /cc1 /host/cc1

# Run with auto-build
run:
	docker run --rm -it -v "$(PWD)":/usr/src/$(PROJECT_NAME) $(DOCKER_IMG)

# Clean
clean:
	docker run --rm -it -v "$(PWD)":/usr/src/$(PROJECT_NAME) $(DOCKER_IMG) \
		cargo clean
	rm -f cc1

.PHONY: all run exec clean