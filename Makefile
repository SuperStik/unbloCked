CC ?= cc
EXE := unbloCked
SRC_DIR := src
SRC := $(wildcard ${SRC_DIR}/*.c)
SRC += $(wildcard ${SRC_DIR}/phys/*.c)
SRC += $(wildcard ${SRC_DIR}/level/*.c)
RES := resources
# TODO: make sure the base name has at least one character
RES_SRC := $(shell find ${RES}/ -name '*.*')

GLSL_SRC := ${SRC_DIR}/glsl
GLSL_VERT := $(wildcard ${GLSL_SRC}/*.vert)
GLSL_FRAG := $(wildcard ${GLSL_SRC}/*.frag)
OBJ_DIR = ${OUT_DIR}/objects
OBJ = $(patsubst src/%.c,${OBJ_DIR}/%.o,${SRC})
GLSL_DIR = ${OUT_DIR}/glsl
RES_DIR = ${OUT_DIR}/resources
RES_OUT = $(patsubst ${RES}/%,${RES_DIR}/%,${RES_SRC})

GLSL_VERT_OUT = $(patsubst ${GLSL_SRC}/%.vert,${GLSL_DIR}/%.vert,${GLSL_VERT})
GLSL_FRAG_OUT = $(patsubst ${GLSL_SRC}/%.frag,${GLSL_DIR}/%.frag,${GLSL_FRAG})

override LIB += m png pthread sdl3 z
override FRAMEWORK += OpenGL
override LIB_PATH += /usr/local/lib
override INCL_PATH += src /usr/local/include

LIB_FL := $(patsubst %,-l%,${LIB})
FRAMEWORK_FL := $(patsubst %, -framework %, ${FRAMEWORK})
LIB_PATH_FL := $(patsubst %, -L%, ${LIB_PATH})
INCL_PATH_FL := $(patsubst %, -I%, ${INCL_PATH})

.PHONY: all clean

OUT_DIR := build
OUT := ${OUT_DIR}/${EXE}

O ?= 2

override CCFLAGS += -flto -funsafe-math-optimizations -fno-math-errno

all: ${OUT} ${GLSL_VERT_OUT} ${GLSL_FRAG_OUT} ${RES_OUT}
	echo ${RES_OUT}

${OUT}: ${OBJ}
	${CC} $^ -O$O -o $@ ${LIB_PATH_FL} ${LIB_FL} ${FRAMEWORK_FL} ${CCFLAGS}

${OBJ_DIR}/%.o: ${SRC_DIR}/%.c ${OBJ_DIR}
	${CC} $< -O$O -o $@ -c ${INCL_PATH_FL} ${CCFLAGS}

${GLSL_DIR}/%.vert: ${GLSL_SRC}/%.vert ${GLSL_DIR}
	cp -f $< $@

${GLSL_DIR}/%.frag: ${GLSL_SRC}/%.frag ${GLSL_DIR}
	cp -f $< $@

${RES_DIR}/%: ${RES}/% ${RES_DIR}
	@mkdir -p `dirname $@`
	echo $<
	cp -f $< $@

${OBJ_DIR}:
	mkdir -p $@
	mkdir $@/phys
	mkdir $@/level

${GLSL_DIR}:
	mkdir -p $@

${RES_DIR}:
	mkdir -p $@

${OUT_DIR}:
	mkdir $@

clean:
	rm -fr ${OUT_DIR}
