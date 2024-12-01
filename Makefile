# Compiler and flags
CC = g++
TARGET = $(BUILD_DIR)/jungle_king
LDFLAGS = -lSDL2 -lSDL2_mixer -lSDL2_ttf
OPTFLAGS = -O2 -Wall
BUILD_DIR = build

# Files and directories
SRC_DIR = src
ASSETS_DIR = assets
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# Define compiler flags for wasm
ifeq ($(ENV),wasm)

CC = emcc
TARGET = web/index.html
LDFLAGS = --use-port=sdl2 --use-port=sdl2_mixer --use-port=sdl2_ttf --preload-file $(ASSETS_DIR) --shell-file shell.html
CFLAGS = -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2

endif

all: clean create-dirs copy-assets $(TARGET)

create-dirs:
	mkdir -p $(BUILD_DIR)
ifeq ($(ENV),wasm)
	mkdir -p web
endif

# Link object files into the final binary
$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $^ $(LDFLAGS) $(OPTFLAGS)

# Compile .cpp files into .o files in the build directory
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) $(OPTFLAGS) -c $< -o $@

# Copy assets to the build directory
copy-assets: create-dirs
	cp -r $(ASSETS_DIR) $(BUILD_DIR)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# run binary
run:
	./build/jungle_king

# Include dependency files - use for debugging
#-include $(OBJS:.o=.d)
