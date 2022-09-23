C_FLAGS += -Wall -g
AR_FLAGS += -r
CC=$(MVTOOL_PREFIX)gcc $(INCLUDES) $(C_FLAGS) -c
ELF=client
SRC=$(shell find -name '*.c')
object=$(SRC:.c=.o)
$(ELF):$(object)
	gcc $^ -o $@
$(object):
.PHONY:clean
clean:
	rm -f $(object)  $(ELF)
