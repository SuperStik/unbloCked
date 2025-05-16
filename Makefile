CC ?= cc
EXE := unbloCked
SRC_DIR := src
SRC := $(wildcard ${SRC_DIR}/*.c)
SRC += $(wildcard ${SRC_DIR}/phys/*.c)

GLSL_SRC := ${SRC_DIR}/glsl
GLSL_VERT := $(wildcard ${GLSL_SRC}/*.vert)
GLSL_FRAG := $(wildcard ${GLSL_SRC}/*.frag)
OBJ_DIR = ${OUT_DIR}/objects
OBJ = $(patsubst src/%.c,${OBJ_DIR}/%.o,${SRC})
GLSL_DIR = ${OUT_DIR}/glsl
GLSL_VERT_OUT = $(patsubst ${GLSL_SRC}/%.vert,${GLSL_DIR}/%.vert,${GLSL_VERT})
GLSL_FRAG_OUT = $(patsubst ${GLSL_SRC}/%.frag,${GLSL_DIR}/%.frag,${GLSL_FRAG})

override LIB += sdl3 m
override FRAMEWORK += OpenGL
LIB_FL := $(patsubst %,-l%,${LIB})
FRAMEWORK_FL := $(patsubst %, -framework %, ${FRAMEWORK})

.PHONY: all clean

OUT_DIR := build
OUT := ${OUT_DIR}/${EXE}

O ?= 2

override CCFLAGS += -flto

all: ${OUT} ${GLSL_VERT_OUT} ${GLSL_FRAG_OUT}

${OUT}: ${OBJ}
	${CC} $^ -O$O -o $@ ${LIB_FL} ${FRAMEWORK_FL} ${CCFLAGS}

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c ${OBJ_DIR}
	${CC} $< -O$O -o $@ -c ${CCFLAGS}

${GLSL_DIR}/%.vert: ${GLSL_SRC}/%.vert ${GLSL_DIR}
	cp -f $< $@

${GLSL_DIR}/%.frag: ${GLSL_SRC}/%.frag ${GLSL_DIR}
	cp -f $< $@

${OBJ_DIR}:
	mkdir -p $@
	mkdir $@/phys

${GLSL_DIR}:
	mkdir -p $@

${OUT_DIR}:
	mkdir $@

clean:
	rm -fr ${OUT_DIR}
