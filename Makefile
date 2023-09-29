ROOT_DIR := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
BUILD_DIR := build

# ===========================================================

all: smoke

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake -G "Unix Makefiles" $(ROOT_DIR)

clean:
	-rm -r $(BUILD_DIR)
	-find $(ROOT_DIR) -maxdepth 1 -xtype l -delete

matrices: | $(BUILD_DIR)
	cd $(BUILD_DIR) && $(MAKE) $@
	ln -nsf $(BUILD_DIR)/src/$@/$@ $@

smoke: matrices
	cd smoke_test && ./smoke_test.sh ../matrices

lint: | $(BUILD_DIR)
	find src/ -name '*.cpp' -o -name '*.h' -o -name '*.hpp' | xargs clang-tidy -p $(BUILD_DIR)

# ===========================================================

.DEFAULT_GOAL := all
.PHONY: all clean matrices smoke lint
