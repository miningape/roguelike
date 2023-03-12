FILES := main.cpp
OUTPUT_FILE := exe

SDL_DIR := /opt/homebrew/Cellar/sdl2/2.26.4

COMPILER := clang++
INCLUDES := include ${SDL_DIR}/include
LIBS := lib ${SDL_DIR}/lib
LINKER_FLAGS := SDL2
COMPILER_FLAGS := -o ${OUTPUT_FILE} -std=c++20

# Functions to make including stuff easier
prepend = ${foreach wrd,${$(1)},$(2)${wrd}}
includes := ${call prepend,INCLUDES,-I}
libraries := ${call prepend,LIBS,-L}
links := ${call prepend,LINKER_FLAGS,-l}

# dependencies are rules/files that can be associated with fx a file online that needs to be downloaded
dependencies := include/entt.hpp

# Default rule that generates executable and calls it
run: ${OUTPUT_FILE}
	./${OUTPUT_FILE}

# Logic that runs compiler and produces the executable
${OUTPUT_FILE}: ${FILES} ${dependencies}
	${COMPILER} ${FILES} ${COMPILER_FLAGS} ${includes} ${libraries} ${links}

include/:
	mkdir include
lib/:
	mkdir lib

# Rule level dependencies
include/entt.hpp: include/
	curl https://raw.githubusercontent.com/skypjack/entt/master/single_include/entt/entt.hpp > include/entt.hpp
