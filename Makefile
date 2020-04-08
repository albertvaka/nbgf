
EXEC	= bin/gaem2020

SRC	= $(wildcard src/*.cpp)
OBJ	= $(patsubst src/%, obj/%.o, $(SRC))

IMGUI_SRC = $(wildcard imgui*/*.cpp)
IMGUI_OBJ = $(patsubst imgui/%, obj/imgui/%.o, $(patsubst imgui_sfml/%, obj/imgui_sfml/%.o, $(IMGUI_SRC)))

OPTIM	= 0
DEBUG	= 1
PROFILE	= 0

# Bash so we can use curly braces expansion
SHELL = bash
CXX	= g++

CFLAGS	= -pipe -I imgui -I imgui_sfml -std=c++17 -Wall -Wno-unused-parameter -Wno-reorder $(PROFILEFLAGS) $(DEBUGFLAGS) -O$(strip $(OPTIM))
LDFLAGS	= -pipe -std=c++17 -lsfml-graphics -lsfml-system -lsfml-audio -lsfml-window $(OPENGLFLAGS) $(PROFILEFLAGS) $(DEBUGFLAGS) -O$(strip $(OPTIM))

ifeq ($(shell uname),Linux)
	OPENGLFLAGS=-lGL
else
	OPENGLFLAGS=-framework OpenGL
endif

ifeq ($(strip $(PROFILE)),1)
	PROFILEFLAGS=-pg
endif
ifeq ($(strip $(DEBUG)),1)
	#DEBUGFLAGS=-DDEBUG -ggdb3
	DEBUGFLAGS=-D_DEBUG -g
endif

$(EXEC): $(OBJ) $(IMGUI_OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) $(IMGUI_OBJ) -o $(EXEC)

obj/main.cpp.o: src/main.cpp src/*.h Makefile
	@mkdir -p obj
	$(CXX) $(CFLAGS) -c $< -o $@

obj/%.cpp.o: src/%.cpp src/*.h Makefile
	@mkdir -p obj/
	$(CXX) $(CFLAGS) -c $< -o $@

obj/imgui_sfml/%.cpp.o: imgui_sfml/%.cpp imgui_sfml/*.h Makefile
	@mkdir -p obj/imgui_sfml
	$(CXX) $(CFLAGS) -c $< -o $@

obj/imgui/%.cpp.o: imgui/%.cpp imgui/*.h Makefile
	@mkdir -p obj/imgui
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(IMGUI_OBJ) $(EXEC)
