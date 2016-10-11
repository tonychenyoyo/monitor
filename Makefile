CC=gcc
CFLAGS=-I ./lib
LIB=-L ./lib -lpthread -liniparser 

PROC=process.o main.o
monitor:$(PROC)
	$(CC) $(CFLAGS) $(PROC) -o $@ $(LIB) 
process.o:process.c process.h vns_list.h
	$(CC) -c $< -o $@
main.o:main.c
	$(CC) -c $< -o $@
clean:
	rm -rf *.o
	rm -rf monitor
