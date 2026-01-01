BUILD_DIR=build
BIN_MAIN=$(BUILD_DIR)/DBacktest
BIN_TEST=$(BUILD_DIR)/tests/DBacktest_TEST

ARGS=crypto coinbase sol-usd 1m output.txt

.PHONY: all build run test clean cmake

all: build

cmake:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake ..

build: cmake
	$(MAKE) -C $(BUILD_DIR)

run: build
	@echo "--- Running DBacktest ---"
	./$(BIN_MAIN) $(ARGS)
	@echo "--- End of DBacktest ---"

test: build
	@echo "--- Running Tests ---"
	./$(BIN_TEST)
	@echo "--- End of Tests ---"

clean:
	rm -rf $(BUILD_DIR)
	@echo "Build directory removed."
