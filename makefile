CC:=gcc
LIST:=$(wildcard *.c)#1.c 2.c 3.c
EXE:=$(patsubst %.c,%,$(LIST))
#all:$(EXE)#循环
#	for i in $(EXE);do $(CC) $$i.c -o $$i;done
all:$(EXE)
%:%.c
	$(CC) $^ -o $@  -lpthread
.PHONY:clean rebuild
rebuild:clean all
clean:
	$(RM) $(EXE)
