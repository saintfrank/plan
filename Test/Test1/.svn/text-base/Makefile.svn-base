############################################
############################################
## This Makefile is in charge of building  ##
## the test for the format utility.       ##
############################################
############################################
#


# Header file for cunit
INC = -I /usr/include/CUnit/ -I ../../Src

# Compiler flags
CFLAGS = -Wall -pedantic -g

# Compiler
CC = gcc

# libreria da testare
LIB_PATH = -L ../../lib/

.PHONY : test clean

format_test: format_test.o format_suite.o
	$(CC)  $(LIB_PATH) -o format_test $^ -lcunit -ldplan

format_test.o:  format_test.c  format.h
	$(CC) $(CFLAGS) $(INC) -c $<

format_suite.o: format_suite.c  format.h
	$(CC) $(CFLAGS) $(INC) -c $<

# Target di testing
test: format_test
	-rm test1.dat
	-rm test2.dat
	cp Dati/test1.dat .
	chmod 644 test1.dat
	./format_test
	@echo -e "\n\n\a *** Test format superato! ***\n"	


clean:
	rm -f *.o
	rm -f format_test
	rm -f *~
