EXEC        := slipchord
DEBUG       := yes
CC          := mpicc
CFLAGS      := -W -Wall
LDFLAGS     :=
NP          ?= 5
MPIRUN      := mpirun
MPIFLAGS    += --oversubscribe -np $(NP)

ifeq ($(DEBUG),yes)
	CFLAGS += -g
endif

.PHONY: all
all: $(EXEC)

slipchord: slipchord.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run
run:
	$(MPIRUN) $(MPIFLAGS) $(EXEC)

.PHONY: clean
clean:
	rm -rf $(EXEC)
	rm -rf *.o
