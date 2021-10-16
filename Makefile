# Common
CC = gcc -Wall
INC = -I/usr/src/linux-headers-$(shell uname -r)/include
KERNEL_SRC := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
EXTRA_CFLAGS := -I$(PWD)/src

BUILD_DIR := build
SOURCES_DIR := src

# Get sources from ./src directory
MODULE_NAME = hack_syscall_tbl_module
SOURCES := $(SOURCES_DIR)/hook_funcs.c \
           $(SOURCES_DIR)/module_main.c \
           $(SOURCES_DIR)/syscall_utils.c
$(MODULE_NAME)-objs = $(SOURCES:.c=.o)

obj-m := $(MODULE_NAME).o

all: PREPARE
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD)/$(BUILD_DIR) src=$(PWD) modules

clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD)/$(BUILD_DIR) src=$(PWD) clean
	rm -rf $(PWD)/$(BUILD_DIR)

install:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD)/$(BUILD_DIR) src=$(PWD) modules_install

PREPARE: $(SOURCES)
	mkdir -p "$(BUILD_DIR)"
	mkdir -p "$(BUILD_DIR)/$(SOURCES_DIR)"
	touch $(BUILD_DIR)/Makefile

