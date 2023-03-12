SRC_DIR := $(CURDIR)
OUT_DIR := /var/tmp/kodanuki
BIN_DIR := $(OUT_DIR)/build
PROJECT := splash

help:
%:
	@ python -B target/build.py $(SRC_DIR) $(OUT_DIR) $(BIN_DIR) $(PROJECT) $@
