EXEC        := test_ordre_cyclique
DEBUG       := yes
CC          := gcc
CFLAGS      := -W -Wall

ifeq ($(DEBUG),yes)
	CFLAGS += -g
endif

.PHONY: all
all: $(EXEC) run

$(EXEC): $(EXEC).o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run
run: $(EXEC)
	./$(EXEC)

.PHONY: clean
clean:
	rm -rf $(EXEC)
	rm -rf *.o
