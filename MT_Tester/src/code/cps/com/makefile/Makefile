#模块Makefile

UNIT_SRCS_EXCLUDE +=
UNIT_SRCS = $(notdir $(filter-out $(addprefix ../source/,$(UNIT_SRCS_EXCLUDE)),$(wildcard ../source/*.c)))

MODULE_MACROS = ${PROJECT_MACROS}
MODULE_MACROS +=

MODULE_INCLUDES = ${PROJECT_INCLUDES}
MODULE_INCLUDES += ../../boot/include
MODULE_INCLUDES += ../../com/include
MODULE_INCLUDES += ../../dbg/include
MODULE_INCLUDES += ../../devm/include
MODULE_INCLUDES += ../../fs/include
MODULE_INCLUDES += ../../mm/include
MODULE_INCLUDES += ../../replant/include
MODULE_INCLUDES += ../../sysctl/include
MODULE_INCLUDES += ../../tm/include
MODULE_INCLUDES += ../../verm/include
MODULE_INCLUDES += ../../vk/include
MODULE_INCLUDES += ../../vos/include


###############################################################################
MODULE_PATH = $(shell pwd)
MODULE_NAME = $(notdir $(MODULE_PATH:%/makefile=%))

MODULE_OBJ = ${MODULE_NAME}.o
MODULE_OUTPUT_PATH = ${OUTPUT}/${MODULE_NAME}
MODULE_OUTPUT = ${MODULE_OUTPUT_PATH}/../${MODULE_OBJ}

UNIT_OBJS = $(notdir $(UNIT_SRCS:%.c=%.o))
UNIT_OUTPUTS = $(addprefix $(MODULE_OUTPUT_PATH)/,${UNIT_OBJS})
UNIT_DEPENDS = $(UNIT_OUTPUTS:%.o=%.d)

all: ${MODULE_OUTPUT_PATH} ${MODULE_OUTPUT}

${MODULE_OUTPUT_PATH}:
	@${TOOL_MKDIR} "$@"

${MODULE_OUTPUT}: ${UNIT_OUTPUTS}
	@${TOOL_ECHO} 'Building target: $@'
	${TOOL_LD_PARTIAL} ${PROJECT_LD_PARTIAL_OPTION} $^ -o "$@"
	@${TOOL_ECHO} 'Finished building target: $@'
	@${TOOL_ECHO} ' '

${UNIT_OUTPUTS}:$(MODULE_OUTPUT_PATH)/%.o:../source/%.c
	@${TOOL_ECHO} 'Building target: $@'
	${TOOL_CC} ${PROJECT_CC_OPTION} $(addprefix -D, ${MODULE_MACROS}) $(addprefix -I, ${MODULE_INCLUDES}) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@${TOOL_ECHO} 'Finished building target: $@'
	@${TOOL_ECHO} ' '

clean:
	${TOOL_RM} ${MODULE_OUTPUT} ${MODULE_OUTPUT_PATH}

-include ${UNIT_DEPENDS}

.PHONY: all clean
