
EXEC	= bin/gaem2020

SRC	= $(wildcard src/*.cpp)
OBJ	= $(patsubst src/%, obj/%.o, $(SRC))

ENGINE_SRC	= $(wildcard engine/*.cpp)
ENGINE_OBJ	= $(patsubst engine/%, obj/engine/%.o, $(ENGINE_SRC))

DEP_SRC = $(shell find vendor/ -type f -name '*.cpp' -o -name '*.c' ! -path 'vendor/glew/*')
DEP_OBJ = $(patsubst vendor/%, obj/vendor/%.o, $(DEP_SRC))
DEP_INCLUDE = $(patsubst vendor/%, -I vendor/%, $(shell find vendor -maxdepth 2 -path \*\include ! -path vendor/SDL2/include) $(shell find vendor -mindepth 1 -maxdepth 1 ! -path vendor/glew -type d '!' -exec test -e "{}/include" ';' -print ))

OPTIM     = 0
DEBUG     = 1
PROFILE   = 0
IMGUI     = 1
WEBGL_VER = 2

# Bash so we can use curly braces expansion
SHELL = bash

#NOTE: Dynamic casts are disabled by fno-rtti
CFLAGS = -pipe $(shell sdl2-config --cflags) -I./engine $(DEP_INCLUDE) -Wall -Wno-unused-parameter $(PROFILEFLAGS) $(DEBUGFLAGS) $(IMGUIFLAGS) -O$(strip $(OPTIM)) $(PLATFORM_CFLAGS)
CXXFLAGS = $(CFLAGS) -std=c++17 -fno-rtti -fno-exceptions -Wno-reorder
LDFLAGS	 = $(CXXFLAGS) $(shell sdl2-config --libs) -lSDL2_ttf -lSDL2_mixer $(PLATFORM_LDFLAGS)

ifdef EMSCRIPTEN
	OUT_FILE=$(EXEC).js
	ifeq ($(strip $(WEBGL_VER)),2)
		WEBGL_FLAGS=-s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2 -DSDL_GPU_DISABLE_GLES_2 -DIMGUI_IMPL_OPENGL_ES3
	else
		WEBGL_FLAGS=-s MIN_WEBGL_VERSION=1 -s MAX_WEBGL_VERSION=1 -DSDL_GPU_DISABLE_GLES_3 -DIMGUI_IMPL_OPENGL_ES2
	endif
	PLATFORM_CFLAGS=-Oz -DSDL_GPU_DISABLE_OPENGL -DSDL_GPU_DISABLE_GLES_1 -s USE_SDL_TTF=2 -s USE_SDL_MIXER=2 -s USE_OGG -s USE_VORBIS -s ALLOW_MEMORY_GROWTH=1 --preload-file bin/data@/data --use-preload-plugins $(WEBGL_FLAGS)
	PLATFORM_LDFLAGS=
else
	OUT_FILE=$(EXEC)
	ifeq ($(shell uname),Linux)
		PLATFORM_CFLAGS=-DSDL_GPU_DISABLE_GLES
		PLATFORM_LDFLAGS=-lGL -lGLEW
	else
		# MacOS
		PLATFORM_CFLAGS=-DSDL_GPU_DISABLE_OPENGL_4 -DSDL_GPU_DISABLE_GLES
		PLATFORM_LDFLAGS=-framework OpenGL -lGLEW
	endif
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

$(EXEC): $(OBJ) $(ENGINE_OBJ) $(DEP_OBJ) Makefile
	$(CXX) $(LDFLAGS) $(OBJ) $(ENGINE_OBJ) $(DEP_OBJ) -o $(OUT_FILE)

obj/main.cpp.o: engine/main.cpp src/*.h engine/*.h Makefile
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/engine/%.cpp.o: engine/%.cpp engine/*.h src/assets.h src/tiledexport.h Makefile
	@mkdir -p obj/engine
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.cpp.o: src/%.cpp engine/*.h src/*.h Makefile
	@mkdir -p obj
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/vendor/%.c.o: vendor/%.c $(shell find vendor/ -type f -name '*.h') Makefile
	@mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS) -c $< -o $@

obj/vendor/%.cpp.o: vendor/%.cpp $(shell find vendor/ -type f -name '*.h') Makefile
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(ENGINE_OBJ) $(DEP_OBJ) $(OUT_FILE)

www:
	emmake $(MAKE)
