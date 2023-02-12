SRC_DIR := $(CURDIR)
OUT_DIR := /var/tmp/kodanuki
BIN_DIR := $(OUT_DIR)/build
BUILDER := @ python -B target/build.py

help:
	$(BUILDER) $(SRC_DIR) $(OUT_DIR) $(BIN_DIR) --target help

%:
	$(BUILDER) $(SRC_DIR) $(OUT_DIR) $(BIN_DIR) --target $@
