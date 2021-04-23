ASOBJS = lexer.o parser.o ir.o ir_preproc.o ir_fixup.o ir_emit.o
SHAREDOBJS = param.o data_support.o charsets.o commands.o data.o
OBJS = $(ASOBJS) $(SHAREDOBJS)
TARGETS = script-as script-dis

CFLAGS += -Wall -g

all: $(TARGETS)

script-as: $(ASOBJS) $(SHAREDOBJS)

script-dis: $(SHAREDOBJS)

clean:
	$(RM) $(OBJS) $(TARGETS)
