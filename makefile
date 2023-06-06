SRC_DIR := $(CURDIR)
OUT_DIR := /var/tmp/kodanuki
BIN_DIR := $(OUT_DIR)/build
PROJECT := unittest

help:
%:
	@ python -B target/build.py $(SRC_DIR) $(OUT_DIR) $(BIN_DIR) $(PROJECT) $@

install:
	@ python -m pip install -r target/requirements.txt
