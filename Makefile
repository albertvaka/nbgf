
EXEC	= bin/gaem

.PHONY = clean run www

SRC	= $(wildcard src/*.cpp)
OBJ	= $(patsubst src/%, $(OBJ_DIR)/%.o, $(SRC))

ENGINE_SRC	= $(wildcard engine/*.cpp)
ENGINE_OBJ	= $(patsubst engine/%, $(OBJ_DIR)/engine/%.o, $(ENGINE_SRC))

GENERATED_SRC	= $(wildcard generated/*.cpp)
GENERATED_OBJ	= $(patsubst generated/%, $(OBJ_DIR)/generated/%.o, $(GENERATED_SRC))

#SDL and GLEW are vendored in Windows but not in MacOS/Linux
DEP_SRC = $(shell find vendor -type f \( -name '*.cpp' -o -name '*.c' \) -not -path 'vendor/glew/*')
DEP_OBJ = $(patsubst vendor/%, $(OBJ_DIR)/vendor/%.o, $(DEP_SRC))
DEP_INCLUDE = $(patsubst vendor/%, -I vendor/%, $(shell find vendor -maxdepth 2 -path \*\include -not -path vendor/SDL2/include) $(shell find vendor -mindepth 1 -maxdepth 1 -not -path vendor/glew -type d -not -exec test -e "{}/include" ';' -print ))

OPTIM     ?= 0
DEBUG     ?= 1
PROFILE   ?= 0
IMGUI     ?= $(DEBUG)

# Bash so we can use curly braces expansion
SHELL = bash

ifeq ($(shell uname),Darwin) # MacOS
	date=gdate
else
	date=date
endif

#NOTE: Dynamic casts are disabled by fno-rtti
CFLAGS = -pipe -I./engine -I./generated $(DEP_INCLUDE) -Wall -Wno-unused-parameter -Werror=return-type $(PROFILEFLAGS) $(DEBUGFLAGS) $(IMGUIFLAGS) -O$(strip $(OPTIM)) $(PLATFORM_CFLAGS)
CXXFLAGS = $(CFLAGS) -std=c++20 -fno-rtti -fno-exceptions -Wno-reorder
LDFLAGS	 = $(CXXFLAGS) -lSDL2_ttf -lSDL2_mixer $(PLATFORM_LDFLAGS)

ifdef EMSCRIPTEN
	OBJ_DIR=emobj
	OUT_FILE=$(EXEC).js
	WEBGL_CFLAGS=-DSDL_GPU_DISABLE_GLES_2 -DIMGUI_IMPL_OPENGL_ES3
	WEBGL_LDFLAGS=-s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2
	PLATFORM_CFLAGS=-DSDL_GPU_DISABLE_OPENGL -DSDL_GPU_DISABLE_GLES_1 -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 -s USE_OGG -s USE_VORBIS $(WEBGL_CFLAGS)
	PLATFORM_LDFLAGS=-s GL_PREINITIALIZED_CONTEXT=1 -lidbfs.js -s EXPORTED_FUNCTIONS='["_main", "_start_main_loop"]' -s EXPORTED_RUNTIME_METHODS='["ccall"]' -s ALLOW_MEMORY_GROWTH=1 $(WEBGL_LDFLAGS) --preload-file bin/data@/data --use-preload-plugins
else
	OBJ_DIR=obj
	OUT_FILE=$(EXEC)
	ifeq ($(shell uname),Darwin) # MacOS
		OS_CFLAGS=-DSDL_GPU_DISABLE_OPENGL_4
		OS_LDFLAGS=-framework OpenGL -framework Foundation
		MACOS_BRIDGE=$(OBJ_DIR)/engine/macos_bridge.mm.o
	else # Linux
		OS_CFLAGS=
		OS_LDFLAGS=-lGL
	endif
	PLATFORM_CFLAGS=$(OS_CFLAGS) -DSDL_GPU_DISABLE_GLES $(shell sdl2-config --cflags) $(shell pkg-config --cflags glew)
	PLATFORM_LDFLAGS=$(OS_LDFLAGS) $(shell sdl2-config --libs) $(shell pkg-config --libs glew)
endif

ifeq ($(strip $(PROFILE)),1)
	PROFILEFLAGS=-pg
endif
ifeq ($(strip $(DEBUG)),1)
	#DEBUGFLAGS=-DDEBUG -ggdb3
	DEBUGFLAGS=-D_DEBUG -g
endif
ifeq ($(strip $(IMGUI)),1)
	IMGUIFLAGS=-D_IMGUI
endif

$(EXEC): $(OBJ) $(ENGINE_OBJ) $(GENERATED_OBJ) $(DEP_OBJ) $(MACOS_BRIDGE) Makefile
	$(CXX) $(OBJ) $(GENERATED_OBJ) $(ENGINE_OBJ) $(DEP_OBJ) $(LDFLAGS) $(MACOS_BRIDGE) -o $(OUT_FILE)

$(OBJ_DIR)/engine/%.cpp.o: engine/%.cpp engine/*.h src/assets.h src/scene_entrypoint.h src/window_conf.h Makefile
	@mkdir -p $(OBJ_DIR)/engine
	$(call time_begin,$@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	$(call time_end,$@)

$(MACOS_BRIDGE): engine/macos_bridge.mm engine/*.h src/window_conf.h Makefile
	@mkdir -p $(OBJ_DIR)/engine
	$(call time_begin,$@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	$(call time_end,$@)

$(OBJ_DIR)/generated/%.cpp.o: generated/%.cpp generated/%.h engine/*.h Makefile
	@mkdir -p $(OBJ_DIR)/generated
	$(call time_begin,$@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	$(call time_end,$@)

$(OBJ_DIR)/%.cpp.o: src/%.cpp engine/*.h $(wildcard generated/*.h) src/*.h Makefile
	@mkdir -p $(OBJ_DIR)
	$(call time_begin,$@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	$(call time_end,$@)

$(OBJ_DIR)/vendor/%.c.o: vendor/%.c $(shell find vendor/ -name '*.h' -o -name '*.inl') Makefile
	@mkdir -p $(shell dirname $@)
	$(call time_begin,$@)
	$(CC) $(CFLAGS) -c $< -o $@
	$(call time_end,$@)

$(OBJ_DIR)/vendor/%.cpp.o: vendor/%.cpp $(shell find vendor/ -name '*.h' -o -name '*.inl') Makefile
	@mkdir -p $(shell dirname $@)
	$(call time_begin,$@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	$(call time_end,$@)

clean:
	$(RM) $(OBJ) $(ENGINE_OBJ) $(GENERATED_OBJ) $(DEP_OBJ) $(OUT_FILE)

www:
	emmake $(MAKE)

run: $(EXEC)
	@$(EXEC)

define time_begin
	@$(date) +%s%3N > $(1).time
endef

define time_end
	@echo "Built $(1) in $$(($$($(date) +%s%3N)-$$(cat $(1).time))) ms"
endef
