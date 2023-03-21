FILES := main.cpp 
WATCH := src/game.hpp src/spritesheet.hpp
OUTPUT_FILE := exe

COMPILER := clang++
INCLUDES := include
LIBS := lib
LINKER_FLAGS := SDL2 SDL2_image
COMPILER_FLAGS := -o ${OUTPUT_FILE} -std=c++20 -Wgnu-designator

# Functions to make including stuff easier
prepend = ${foreach wrd,${$(1)},$(2)${wrd}}
includes := ${call prepend,INCLUDES,-I}
libraries := ${call prepend,LIBS,-L}
links := ${call prepend,LINKER_FLAGS,-l}

# dependencies are are additional rules, but should represent a file, e.g. if it needs to be downloaded from online
dependencies := include/entt.hpp include/SDL2/ lib/

SDL_DIR := /opt/homebrew/Cellar/sdl2/2.26.4
SDL_IMAGE := /opt/homebrew/Cellar/sdl2_image/2.6.3

# Default rule that generates executable and calls it
run: ${OUTPUT_FILE}
	./${OUTPUT_FILE}

debug: ${FILES} ${WATCH} ${dependencies}
	${COMPILER} -fcolor-diagnostics -fansi-escape-codes -g ${FILES} ${COMPILER_FLAGS}  ${includes} ${libraries} ${links}

# Logic that runs compiler and produces the executable
${OUTPUT_FILE}: ${FILES} ${WATCH} ${dependencies}
	${COMPILER} ${FILES} ${COMPILER_FLAGS} ${includes} ${libraries} ${links}

include/:
	mkdir include
lib/:
	mkdir lib
	rsync -rva --exclude cmake --exclude pkgconfig ${SDL_DIR}/lib/ lib
	rsync -rva --exclude cmake --exclude pkgconfig ${SDL_IMAGE}/lib/ lib

# Rule level dependencies
include/entt.hpp: include/
	curl https://raw.githubusercontent.com/skypjack/entt/master/single_include/entt/entt.hpp > include/entt.hpp

include/SDL2/: include/
	mkdir include/SDL2
	cp ${SDL_DIR}/include/SDL2/* include/SDL2
	cp ${SDL_IMAGE}/include/SDL2/* include/SDL2
	
	
