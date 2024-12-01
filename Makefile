
EXEC	= bin/gaem

.PHONY = clean run www

SRC	= $(wildcard src/*.cpp)
OBJ	= $(patsubst src/%, obj/%.o, $(SRC))

ENGINE_SRC	= $(wildcard engine/*.cpp)
ENGINE_OBJ	= $(patsubst engine/%, obj/engine/%.o, $(ENGINE_SRC))

GENERATED_SRC	= $(wildcard generated/*.cpp)
GENERATED_OBJ	= $(patsubst generated/%, obj/generated/%.o, $(GENERATED_SRC))

#SDL and GLEW are vendored in Windows but not in MacOS/Linux
DEP_SRC = $(shell find vendor -type f \( -name '*.cpp' -o -name '*.c' \) -not -path 'vendor/glew/*')
DEP_OBJ = $(patsubst vendor/%, obj/vendor/%.o, $(DEP_SRC))
DEP_INCLUDE = $(patsubst vendor/%, -I vendor/%, $(shell find vendor -maxdepth 2 -path \*\include -not -path vendor/SDL2/include) $(shell find vendor -mindepth 1 -maxdepth 1 -not -path vendor/glew -type d -not -exec test -e "{}/include" ';' -print ))

OPTIM     ?= 0
DEBUG     ?= 1
PROFILE   ?= 0
IMGUI     ?= $(DEBUG)
WEBGL_VER ?= 2

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
	OUT_FILE=$(EXEC).js
	ifeq ($(strip $(WEBGL_VER)),2)
		WEBGL_CFLAGS=-DSDL_GPU_DISABLE_GLES_2 -DIMGUI_IMPL_OPENGL_ES3
		WEBGL_LDFLAGS=-s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2
	else
		WEBGL_CFLAGS=-DSDL_GPU_DISABLE_GLES_3 -DIMGUI_IMPL_OPENGL_ES2
		WEBGL_LDFLAGS=-s MIN_WEBGL_VERSION=1 -s MAX_WEBGL_VERSION=1
	endif
	PLATFORM_CFLAGS=-DSDL_GPU_DISABLE_OPENGL -DSDL_GPU_DISABLE_GLES_1 -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 -s USE_OGG -s USE_VORBIS --preload-file bin/data@/data --use-preload-plugins $(WEBGL_CFLAGS)
	PLATFORM_LDFLAGS=-lidbfs.js -s EXPORTED_FUNCTIONS='["_main", "_start_main_loop"]' -s EXPORTED_RUNTIME_METHODS='["ccall"]' -s ALLOW_MEMORY_GROWTH=1 $(WEBGL_LDFLAGS)
else
	OUT_FILE=$(EXEC)
	ifeq ($(shell uname),Darwin) # MacOS
		OS_CFLAGS=-DSDL_GPU_DISABLE_OPENGL_4
		OS_LDFLAGS=-framework OpenGL
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

$(EXEC): $(OBJ) $(ENGINE_OBJ) $(GENERATED_OBJ) $(DEP_OBJ) Makefile
	$(CXX) $(OBJ) $(GENERATED_OBJ) $(ENGINE_OBJ) $(DEP_OBJ) $(LDFLAGS) -o $(OUT_FILE)

obj/engine/%.cpp.o: engine/%.cpp engine/*.h src/assets.h src/scene_entrypoint.h src/window_conf.h Makefile
	@mkdir -p obj/engine
	$(call time_begin,$@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	$(call time_end,$@)

obj/generated/%.cpp.o: generated/%.cpp generated/%.h engine/*.h Makefile
	@mkdir -p obj/generated
	$(call time_begin,$@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	$(call time_end,$@)

obj/%.cpp.o: src/%.cpp engine/*.h $(wildcard generated/*.h) src/*.h Makefile
	@mkdir -p obj
	$(call time_begin,$@)
	$(CXX) $(CXXFLAGS) -c $< -o $@
	$(call time_end,$@)

obj/vendor/%.c.o: vendor/%.c $(shell find vendor/ -name '*.h' -o -name '*.inl') Makefile
	@mkdir -p $(shell dirname $@)
	$(call time_begin,$@)
	$(CC) $(CFLAGS) -c $< -o $@
	$(call time_end,$@)

obj/vendor/%.cpp.o: vendor/%.cpp $(shell find vendor/ -name '*.h' -o -name '*.inl') Makefile
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
