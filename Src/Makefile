###############################################################
###############################################################
## Makefile progetto finale lcs08 
##
###############################################################
###############################################################

#File(s) *.c *.h README relazione etc (da consegnare)
FILE_DA_CONSEGNARE = dserver.c dplan.c 

# Compiler flags
CFLAGS = -Wall -pedantic -g

# Compiler
CC = gcc

#directory librerie
LIBDIR = ../lib

# nome libreria
LIBNAME = libdplan.a

.PHONY: all clean lib test1 test2 test3 testfinale consegna docu testiniziale



all: dserver dplan
	@echo -e 'MAKE ALL completato\a'

clean:
	-rm -fr *.o *~  

# Lista degli object file da inserire nella libreria libdplan.a (***DA COMPLETARE***)
OBJS = 	lbase.o llist.o\


# Target per la creazione della libreria 
lib: $(OBJS)
	ar -r $(LIBNAME) $(OBJS)
	cp $(LIBNAME) $(LIBDIR)


# Target per la creazione degli eseguibili "dserver" e "dplan"
# (***DA COMPLETARE/MODIFICARE***)
dserver: dserver.o lcscom.o  
	$(CC) $(CFLAGS) -L$(LIBDIR) $^ -o dserver -lpthread -ldplan

dplan: dplan.o lcscom.o
	$(CC) $(CFLAGS) -L$(LIBDIR) $^ -o dplan -ldplan







lcscom.o: lcscom.c lcscom.h 
	$(CC) $(CFLAGS) $(INC) -c $<





# NON MODIFICARE
# crea la documentazione html
docu:
	make -C ../Doc 

# invoca i test cunit
testiniziale:
	make lib
	make -C ../Test/Test1 test
	make -C ../Test/Test2 test

# test parsing
test1: dplan
	./testparse 1> errparse 2> .test.err
	diff errparse errparse.check
	@echo -e "\a\n\t\t *** Test parsing superato! ***\n"

# test funzionale
test2: dserver dplan
	-killall dserver 
	-rm -fr tmp/
	-mkdir tmp
	-rm -fr tmp2/
	-mkdir tmp2
	cp DATA/test* tmp2/
	chmod 644 tmp2/*
	./dserver tmp2 1> .server.out 2> .server.err &
	./testfunz 1> outfunz 2> .test.err
	diff outfunz outfunz.check
	@echo -e "\a\n\t\t *** Test funzionale superato! ***\n"

# test parallelo
test3: dserver dplan
	-killall -w dserver 
	-rm -fr tmp/
	mkdir tmp 
	-rm -fr OUT/
	mkdir OUT
	-rm -fr tmp2/
	mkdir tmp2
	cp DATA/test* tmp2/
	cp DATA/*.check OUT/
	chmod 644 tmp2/*
	./dserver tmp2 1> .server.out 2> .server.err &
	./testpar 2> .test.err
	@echo -e "\a\n\t\t *** Test parallelo superato! ***\n"

testfinale:
	make clean	
	make lib
	make all	
	make -C ../Test/Test1 test
	make -C ../Test/Test2 test
	make test1
	make test2
	make test3
	@echo -e "\a\n\t\t *** Test finale superato! ***\n"


######################################
SUBJECT="lcs08: consegna progetto finale DPLAN"
FILENAME=$(USER)-f3dplan.tar

consegna :
	make testfinale
	./gruppo-check.pl < ./gruppo.txt
	tar -cvf $(FILENAME) gruppo.txt $(FILE_DA_CONSEGNARE) 
	mpack -s $(SUBJECT) $(FILENAME)  susanna@di.unipi.it
	@echo -e "\a\n\t\t*** Progetto Finale CONSEGNATO! ***\n"

