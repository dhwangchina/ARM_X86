#工程总控Makefile

.EXPORT_ALL_VARIABLES:
###############################################################################
#	需要命令行传入的变量
#	TARGET	oct/pc
#	SYSTEM	cygwin/linux
###############################################################################
include ${TARGET}-${SYSTEM}.config

VALGRIND = no
ifneq (${VALGRIND},no)
PROJECT_MACROS += CPSS_MM_DISABLE
endif

PROJECT_TARGET = ${TARGET}
PROJECT_SYSTEM = ${SYSTEM}

#相对于总控makefile目录的源代码相对路径
PROJECT_SRC_PATH = ../src/code
PROJECT_MK_PATH = $(shell pwd)

#工程全局的头文件搜索路径，这里要用绝对路径，子make要用
PROJECT_INCLUDES += ${PROJECT_MK_PATH}/${PROJECT_SRC_PATH}/public/include
PROJECT_INCLUDES += ${PROJECT_MK_PATH}/${PROJECT_SRC_PATH}/cps/public/include
PROJECT_INCLUDES += ${PROJECT_MK_PATH}/${PROJECT_SRC_PATH}/mt_proc/include
PROJECT_INCLUDES += ${PROJECT_MK_PATH}/${PROJECT_SRC_PATH}/ap_proc/include
PROJECT_INCLUDES += ${PROJECT_MK_PATH}/${PROJECT_SRC_PATH}/sta_proc/include

#输出可执行文件名称
PROJECT_NAME = MT_Tester${PROJECT_SUFFIX}

PROJECT_OUTPUT_PATH = ${PROJECT_MK_PATH}/Output/${PROJECT_TARGET}-${PROJECT_SYSTEM}
PROJECT_OUTPUT = ${PROJECT_OUTPUT_PATH}/${PROJECT_NAME}
PROJECT_MAP_OUTPUT = $(basename ${PROJECT_OUTPUT}).map

SUBSYSTEM_OBJS = $(addsuffix .o,$(PROJECT_SUBSYSTEMS))
SUBSYSTEM_OUTPUTS = $(addprefix $(PROJECT_OUTPUT_PATH)/, ${SUBSYSTEM_OBJS})

all:${PROJECT_OUTPUT_PATH} ${PROJECT_OUTPUT} ${PROJECT_MAP_OUTPUT}
	@${TOOL_RM} $(SUBSYSTEM_OUTPUTS)

install:
	${TOOL_CP} ${PROJECT_OUTPUT} ${PROJECT_INSTALL_DIR}
	${TOOL_CP} ${PROJECT_MAP_OUTPUT} ${PROJECT_INSTALL_DIR}
ifeq (${VALGRIND},no)
	${TOOL_STRIP} -g ${PROJECT_INSTALL_DIR}/${PROJECT_NAME}
endif

${PROJECT_OUTPUT_PATH}:
	@${TOOL_MKDIR} "$@"

${PROJECT_MAP_OUTPUT}:${PROJECT_OUTPUT}
	@${TOOL_ECHO} 'Building target: $@'
	echo "build-time:$(shell date "+%Y-%m-%d %H-%M-%S")" > "$@"
	${TOOL_NM} -n -C "$<" >> "$@"
	@${TOOL_ECHO} 'Finished building target: $@'
	@${TOOL_ECHO} ' '
	
${PROJECT_OUTPUT}:$(SUBSYSTEM_OUTPUTS)
	@${TOOL_ECHO} 'Building target: $@'
	${TOOL_LD}  ${PROJECT_LD_OPTION} -o "$@" $^ $(addprefix -L, ${PROJECT_LIB_PATHS}) $(addprefix -l, ${PROJECT_LIBS})
	@${TOOL_ECHO} 'Finished building target: $@'
	@${TOOL_ECHO} ' '

$(SUBSYSTEM_OUTPUTS):
	@$(MAKE) -e -C ${PROJECT_SRC_PATH}/$(basename $(notdir $@))/makefile "OUTPUT=${PROJECT_OUTPUT_PATH}"

clean:
	${TOOL_RM} ${PROJECT_MAP_OUTPUT} ${PROJECT_OUTPUT}
	${TOOL_RM} ${PROJECT_OUTPUT_PATH}

.PHONY: all clean install
