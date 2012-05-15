############################################
############################################
## This Makefile is in charge of building  ##
## the test for the format utility.       ##
############################################
############################################
#
# Specifica dove cercare i file di include (in ./Include)
vpath %.h ./../../Src

# Header file for cunit
INC = -I /usr/include/CUnit/ -I ../../Src

# Compiler flags
CFLAGS = -Wall -pedantic -g

# Compiler
CC = gcc

# libreria da testare
LIB_PATH = -L ../../lib/

.PHONY : test clean

load_store_test: load_store_test.o load_store_suite.o
	$(CC)  $(LIB_PATH) -o load_store_test $^ -lcunit -ldplan

load_store_test.o:  load_store_test.c  load_store_suite.h
	$(CC) $(CFLAGS) $(INC) -c $<

load_store_suite.o: load_store_suite.c llist.h
	$(CC) $(CFLAGS) $(INC) -c $<

# Target di testing
test: load_store_test
	-rm test1.dat
	-rm test2.dat
	cp Dati/test1.dat .
	chmod 644 test1.dat
	./load_store_test
	@echo -e "\n\n\a *** Test load_store superato! ***\n"	


clean:
	rm -f *.o
	rm -f load_store_test
	rm -f *~
