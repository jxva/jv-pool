CC=gcc

CFLAGS=-Wall -g -pedantic -pipe -W  -Wpointer-arith -Wno-unused-parameter -Wunused-function -Wunused-variable -Wunused-value -Werror

INCLUDES=-I.

LIBS=

LINKS=

TARGET=jv_pool_main

TEST=jv_pool_test

all:	
	$(CC) $(CFLAGS) $(INCLUDES) -c jv_pool.c -o jv_pool.o
	$(CC) $(CFLAGS) $(INCLUDES) -c jv_pool_main.c -o jv_pool_main.o
	$(CC) $(CFLAGS) $(INCLUDES) -c jv_pool_test.c -o jv_pool_test.o
	
	$(CC) -o $(TARGET) jv_pool_main.o jv_pool.o $(LINKS) $(LIBS)
	$(CC) -o $(TEST) jv_pool_test.o jv_pool.o $(LINKS) $(LIBS)

	@echo 
	@echo Project has been successfully compiled.
	@echo

clean:
	rm -rf $(TARGET) $(TEST) *.depend *.layout bin obj *.o *.stackdump *.exe *.log *~