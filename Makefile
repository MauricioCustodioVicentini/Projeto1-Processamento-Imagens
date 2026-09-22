CC := gcc
PKG_CONFIG := pkg-config

TARGET_NAME := programa
BUILD_DIR := build

SRC := $(wildcard src/*.c)
OBJ := $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRC))
DEP := $(OBJ:.o=.d)

CFLAGS := -std=c99 -Wall -Wextra -MMD -MP

SDL_CFLAGS := $(shell $(PKG_CONFIG) --cflags sdl3 sdl3-image sdl3-ttf)
SDL_LIBS := $(filter-out -mwindows,$(shell $(PKG_CONFIG) --libs sdl3 sdl3-image sdl3-ttf))

CFLAGS += $(SDL_CFLAGS)
LDLIBS := $(SDL_LIBS) -lm


ifeq ($(OS),Windows_NT)

TARGET := $(TARGET_NAME).exe

define CREATE_BUILD_DIR
	if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
endef

define REMOVE_BUILD_DIR
	if exist "$(BUILD_DIR)" rmdir /S /Q "$(BUILD_DIR)"
endef

define REMOVE_TARGET
	if exist "$(TARGET)" del /Q "$(TARGET)"
endef

else

TARGET := $(TARGET_NAME)

define CREATE_BUILD_DIR
	mkdir -p "$(BUILD_DIR)"
endef

define REMOVE_BUILD_DIR
	rm -rf "$(BUILD_DIR)"
endef

define REMOVE_TARGET
	rm -f "$(TARGET)"
endef

endif


.PHONY: all clean rebuild

all: $(TARGET)


$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDLIBS)


$(BUILD_DIR)/%.o: src/%.c
	@$(CREATE_BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	@$(REMOVE_TARGET)
	@$(REMOVE_BUILD_DIR)


rebuild: clean all


-include $(DEP)
