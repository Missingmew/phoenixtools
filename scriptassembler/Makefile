OBJS = lexer.o parser.o ir.o ir_preproc.o ir_fixup.o ir_emit.o

all: assemble-phoenix-script.elf

assemble-phoenix-script.elf: $(OBJS) assemble-phoenix-script.c ../phoenixscript_commands.o ../phoenixscript_data.o ../phoenixscript_charsets.o
	$(CC) -Wall -g -o $@ $^
	
%.o: %.c
	$(CC) -c -Wall -g -o $@ $<
	
clean:
	-rm *.o assemble-phoenix-script.elf