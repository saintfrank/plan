/**
 * \file 
 *
 * \brief Test suite per le funzioni di conversione/matching eventi
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <CUnit/CUnit.h>
#include "CUnit/TestDB.h"
#include "CUnit/Basic.h"
#include "format.h"
#include "lbase.h"

/** Primo file di test*/
#define FILE_TEST1 "./test1.dat"
/** Secondo file di test */
#define FILE_TEST2 "./test2.dat"

/** date per la prova di matchData */
char* date[] = {
    "12-10-2010",
    "01-02-2008",
    "01-08-2008",
    "19-08-2008",
    "11-05-2008",
    "04-06-2008",
    "**-02-2008",
    "**-06-2008",
    "01-*-2008",
    NULL
};

/** pattern per la prova di matchPattern */
char* pattern[] = {
    "macchina",
    "dentista",
    "Pagare",
    "gatto",
    "luigi",
    "8-",
    NULL
};

/** numero di matching attesi per la prova di matchPattern */
int npatternmatch[] = {
    4,
    1,
    5,
    0,
    2,
    2,
};

/** numero di matching attesi per la prova di matchData */
int nmatch[] = {
    0,
    1,
    1,
    1,
    1,
    2,
    1,
    3,
    -10,
};


/** Funzione di inizializzazione della suite */
int success_init(void) { return 0; }
/** Funzione di clean up della suite */
int success_clean(void) { return 0; }

/**
 * Questa funzione legge e scrive un'agenda effettuando controlli 
 * necessari a verificare che il file sia stato formattato correttamente 
 * e che la conversione sia corretta
 *
 * \param fagenda nome del file agenda
 * \param ftest nome del file in cui verranno scritte le voci covertite
 */
void run_testagenda (char *fagenda, char* ftest) {
    char record[LRECORD+2], pbu[MAXNTEST][LRECORD+1], pb[LRECORD+1];
    evento_t* ev[MAXNTEST];
    FILE *fsa;

    /* prossima posizione libera nell'array di eventi */
    int i=0;
    int l,k;

    /*
     * Apertura del file agenda
     */
    fsa = fopen(fagenda, "r");
    CU_ASSERT_PTR_NOT_NULL_FATAL(fsa); 


    /*
     * Lettura e caricamento eventi nell'array
     */
    while(fgets(record,LRECORD + 2,fsa) != NULL) {
	 CU_ASSERT_EQUAL_FATAL(record[LRECORD], '\n');
	 record[LRECORD]='\0';
	 strncpy(pbu[i],record,LRECORD+1);
	 CU_TEST_FATAL(i < MAXNTEST);
	 ev[i]=convertiRecord(record);
	 CU_ASSERT_NOT_EQUAL_FATAL(ev[i],NULL);
	 i++;
	 
    }
    fclose(fsa);
    printf("lette %d voci\n",i);

    /*
     * Trasformazione voci nel formato stringa
     */
    for ( l=0; l<i; l++ ) {
	int ret;
	/* printf("confron ta la mia %s, che viene da %s,%s,%s, con    la sua %s\n", pb, ev[l]->data, ev[l]->utente, ev[l]->descrizione ,pbu[l]); */
	convertiEvento(ev[l],pb);
	/* printf("confronta la mia %s, che viene da %s con la sua %s\n", pb, ev[l]->data ,pbu[l]); */
	ret=strncmp(pbu[l],pb,LRECORD+1);
	CU_ASSERT_EQUAL_FATAL(ret, 0);
    }

    /*
     * Verifica funzione di matchPattern
     */
    
    for (k=0;pattern[k]!=NULL; k++) {
	int cont = 0;
	for(l=0;l<i;l++) {
	    cont+=matchPattern(pattern[k],ev[l]);
/*	    printf(" pattern %s : %d occurrences \n",pattern[k],cont);*/
	}
	 CU_ASSERT_EQUAL_FATAL(cont, npatternmatch[k]); 
    }

    /*
     * Verifica funzione di matchData
     */
    for (k=0;date[k]!=NULL; k++) {
	int cont = 0;
	for(l=0;l<i;l++) {
	    cont+=matchData(date[k],ev[l]);
	}
	/* printf("cont: %d  nmatch %d\n", cont, nmatch[k]); */
	CU_ASSERT_EQUAL_FATAL(cont, nmatch[k]); 
    }

    /* 
     * Test di creazione di una nuova agenda 
     */

    fsa=fopen(ftest,"w");
    CU_ASSERT_PTR_NOT_NULL_FATAL(fsa); 
    for ( l=0; l<i; l++ ) {
	convertiEvento(ev[l],pb);
        CU_ASSERT_NOT_EQUAL_FATAL(fputs(pb,fsa), EOF);
	fputs("\n",fsa);
    }
    fclose(fsa);
    
}


/**
 * \test
 *
 * Testa la formattazione di un file di agenda
 */
void format_test (void)
{
    run_testagenda(FILE_TEST1,FILE_TEST2);
}

/** 
 * Definizione della suite. 
 */
void add_suite_testagenda(void)
{
    CU_pSuite format_suite = NULL;

    format_suite = CU_add_suite("Format Test", success_init, success_clean);
    CU_add_test(format_suite, "Test sull'agenda dat numero 1", format_test);

}

