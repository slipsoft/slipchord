EXECUTABLES := slipchord
CC := mpicc

.PHONY: all
all: $(EXECUTABLES)

slipchord: slipchord.o
	$(CC) $^ -o $@

%.o: %.c
	$(CC) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(EXECUTABLES)
	rm -rf *.o
