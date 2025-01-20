
EXEC	= bin/gaem

.PHONY = clean run www

SRC	= $(wildcard src/*.cpp)
OBJ	= $(patsubst src/%, obj/%.o, $(SRC))

ENGINE_SRC	= $(wildcard engine/*.cpp)
ENGINE_OBJ	= $(patsubst engine/%, obj/engine/%.o, $(ENGINE_SRC))

GENERATED_SRC	= $(wildcard generated/*.cpp)
GENERATED_OBJ	= $(patsubst generated/%, obj/generated/%.o, $(GENERATED_SRC))

DEP_SRC = $(shell find vendor -type f \( -name '*.cpp' -o -name '*.c' \) -not -path 'vendor/raylib/src/external/*' -not -path 'vendor/raylib/src/platforms/*')
DEP_OBJ = $(patsubst vendor/%, obj/vendor/%.o, $(DEP_SRC))
DEP_INCLUDE = -I vendor/raylib/src $(patsubst vendor/%, -I vendor/%, $(shell find vendor -maxdepth 2 -path \*\include) $(shell find vendor -mindepth 1 -maxdepth 1 -not -path 'vendor/glfw/*' -type d -not -exec test -e "{}/include" ';' -print ))

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

#TODO: pass -flto to both linker and compiler to enable link-time optimization

#NOTE: Dynamic casts are disabled by fno-rtti
CFLAGS = -pipe -I engine -I generated $(DEP_INCLUDE) -Wall -Wno-unused-parameter -Werror=return-type -std=c99 $(PROFILEFLAGS) $(DEBUGFLAGS) $(IMGUIFLAGS) -O$(strip $(OPTIM)) $(PLATFORM_CFLAGS) -D_DEFAULT_SOURCE -DPLATFORM_DESKTOP -DGRAPHICS_API_OPENGL_33
CXXFLAGS = $(CFLAGS) -std=c++17 -fno-rtti -fno-exceptions -Wno-reorder
LDFLAGS	 = $(CXXFLAGS) $(PLATFORM_LDFLAGS)

ifdef EMSCRIPTEN
	OUT_FILE=$(EXEC).js
	WEBGL_CFLAGS=-DIMGUI_IMPL_OPENGL_ES3
	WEBGL_LDFLAGS=-s MIN_WEBGL_VERSION=2 -s MAX_WEBGL_VERSION=2
	PLATFORM_CFLAGS=-s USE_GLFW=3 $(WEBGL_CFLAGS)
	PLATFORM_LDFLAGS=-lidbfs.js -s EXPORTED_FUNCTIONS='["_main", "_start_main_loop"]' -s EXPORTED_RUNTIME_METHODS='["ccall"]' -s ALLOW_MEMORY_GROWTH=1 $(WEBGL_LDFLAGS) --preload-file bin/data@/data --use-preload-plugins
else
	OUT_FILE=$(EXEC)
	ifeq ($(shell uname),Darwin) # MacOS
		OS_CFLAGS=
# Hack because raylib includes objective-C code in MacOS
		C_ONLY_FLAGS=-x objective-c
		OS_LDFLAGS=-framework OpenGL -framework OpenAL -framework IOKit -framework CoreVideo -framework Cocoa
	else # Linux
		OS_CFLAGS=-D_GLFW_X11
		OS_LDFLAGS=-lGL
	endif
	PLATFORM_CFLAGS=$(OS_CFLAGS) $(pkg-config --cflags glfw3)
	PLATFORM_LDFLAGS=$(OS_LDFLAGS) $(pkg-config --libs glfw3)
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
	$(CC) $(CFLAGS) $(C_ONLY_FLAGS) -c $< -o $@
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
