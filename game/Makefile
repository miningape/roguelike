FILES := main.cpp
OUTPUT_FILE := exe

SDL_DIR := /opt/homebrew/Cellar/sdl2/2.26.4

COMPILER := clang++
INCLUDES := include ${SDL_DIR}/include
LIBS := lib ${SDL_DIR}/lib
LINKER_FLAGS := SDL2
COMPILER_FLAGS := -o ${OUTPUT_FILE}

# Functions to make including stuff easier
prepend = ${foreach wrd,${$(1)},$(2)${wrd}}
includes := ${call prepend,INCLUDES,-I}
libraries := ${call prepend,LIBS,-L}
links := ${call prepend,LINKER_FLAGS,-l}

# Default rule that generates executable and calls it
run: ${OUTPUT_FILE}
	./${OUTPUT_FILE}

# Logic that runs compiler and produces the executable
${OUTPUT_FILE}: ${FILES}
	${COMPILER} ${FILES} ${COMPILER_FLAGS} ${includes} ${libraries} ${links}
