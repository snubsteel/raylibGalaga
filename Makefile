.PHONY: all clean

# Define required raylib variables
PROJECT_NAME       ?= game
RAYLIB_VERSION     ?= 5.0.0
# Point to our downloaded raylib
RAYLIB_PATH        ?= lib/linux/raylib-5.0_linux_amd64

# Define default options
PLATFORM           ?= PLATFORM_DESKTOP

# Define default C compiler: g++
CC = g++

# Define default make program: make
MAKE = make

# Define compiler flags:
CFLAGS += -Wall -std=c++14 -D_DEFAULT_SOURCE -Wno-missing-braces -g -O0

# Define include paths for required headers
INCLUDE_PATHS = -I. -I$(RAYLIB_PATH)/include

# Define library paths containing required libs.
LDFLAGS = -L. -L$(RAYLIB_PATH)/lib -Wl,-rpath,$(RAYLIB_PATH)/lib

# Define any libraries required on linking
# Libraries for Debian GNU/Linux desktop compiling
LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Define all source files required
SRC_DIR = src
OBJ_DIR = obj

# Define all object files from source files
# Find all cpp files in src/
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Default target entry
all: $(PROJECT_NAME)

# Project target defined by PROJECT_NAME
$(PROJECT_NAME): $(OBJS)
	$(CC) -o $(PROJECT_NAME) $(OBJS) $(CFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS) -D$(PLATFORM)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS) -D$(PLATFORM)

# Clean everything
clean:
	rm -rf $(OBJ_DIR) $(PROJECT_NAME)
	@echo Cleaning done
