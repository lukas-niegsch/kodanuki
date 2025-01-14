SRC_DIR := $(CURDIR)
OUT_DIR := /var/tmp/kodanuki
BIN_DIR := $(OUT_DIR)/build
PROJECT := kodanuki

help:
%:
	@ python -B target/build.py $(SRC_DIR) $(OUT_DIR) $(BIN_DIR) $(PROJECT) $@

install:
	@ git submodule update --init
	@ python -m pip install -r target/requirements.txt
	@ sudo emerge -a $$(< target/gentoo.txt)

profile:
	@ python -B target/build.py $(SRC_DIR) $(OUT_DIR) $(BIN_DIR) unittest run
	@ python -B target/build.py $(SRC_DIR) $(OUT_DIR) $(BIN_DIR) perftest valgrind
