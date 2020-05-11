EXEC        := main
DEBUG       := yes
CC          := mpicc
CFLAGS      := -W -Wall
LDFLAGS     :=
NP          ?= 10
MPIRUN      := mpirun
MPIFLAGS    += --oversubscribe -np $(NP)

ifeq ($(DEBUG),yes)
	CFLAGS += -g
endif

.PHONY: all
all: $(EXEC)

$(EXEC): $(EXEC).o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run
run: $(EXEC)
	$(MPIRUN) $(MPIFLAGS) $(EXEC)

.PHONY: clean
clean:
	rm -rf $(EXEC)
	rm -rf *.o
