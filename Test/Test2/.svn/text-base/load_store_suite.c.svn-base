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
#include "llist.h"

/** numero eventi nel test */
#define NEVENTI 10
/** primo file di test */
#define FILE_TEST1 "./test1.dat"
/** secondo file di test */
#define FILE_TEST2 "./test2.dat"

/** stampa formattata dell'agenda 
    \param agenda puntatore alla agenda da stampare 
*/
static void stampaAgenda(elem_t* agenda) {

  if(agenda == NULL) 
      return ;
  
  printf("EVENTO: ");
  printf("DATA: %s ; ",agenda->ptev->data);
  printf("UTENTE: %s ; ",agenda->ptev->utente);
  printf("DESCRIZIONE: %s\n",agenda->ptev->descrizione);
  stampaAgenda(agenda->next);

}

/** conta gli elementi dell'agenda 
    \param agenda l'agenda di cui contare gli elementi
*/
static int conta (elem_t* agenda) {

  if(agenda == NULL) 
      return 0;
  return (1 + conta(agenda->next));
}




/** date per la prova di matchData -- cerca */
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

/** pattern per la prova di matchPattern -- rimuovi*/
char* pattern[] = {
    "macchina",
    "dentista",
    "Pagare",
    "gatto",
    "luigi",
    "11",
    NULL
};

/** numero di matching attesi per la prova di rimuovi */
int npatternmatch[] = {
    4,
    1,
    1,
    0,
    1,
    1,
};

/** esiti attesi per la prova di cerca */
int nmatch[] = {
    0,
    1,
    1,
    1,
    1,
    2,
    1,
    3,
    -1,
};


/** Funzione di inizializzazione della suite */
int success_init(void) { return 0; }
/** Funzione di clean up della suite */
int success_clean(void) { return 0; }

/**
 * Questa funzione legge un'agenda da file,
 * la inserisce nella lista di eventi, 
 * poi cerca dei pattern e manipola la lista e scrive il risultato in un
 * nuovo file 

 * \param fagenda nome del file agenda
 * \param ftest nome del file in cui verranno scritte le voci covertite
 */
void run_testloadstore (char *fagenda, char* ftest) {
    elem_t* database=NULL,*database1=NULL,*tmp, *tmp1;
    elem_t* risultato;
    int i,k,r,partial;
    FILE *fsa;
    
    /*
     * Apertura del file agenda
     */
    fsa = fopen(fagenda, "r");
    CU_ASSERT_PTR_NOT_NULL_FATAL(fsa); 
    
	printf("check  1\n");

    /*
     * Lettura e caricamento eventi nella lista
     */
    i=0;
    i=loadAgenda(fsa,&database);
    printf("Letti %d record.\n",i);
    stampaAgenda(database);
    CU_ASSERT_EQUAL_FATAL(i, NEVENTI);
    fclose(fsa);
	
	

    /*
     * Cerco gli eventi contenenti data[]
     */
      for (k=0;date[k]!=NULL; k++) {
	 r=cerca(date[k],database,&risultato);	
	 printf("r %d aspettato %d, la data è %s\n", r, nmatch[k],date[k]); 
	 CU_ASSERT_EQUAL_FATAL(r, nmatch[k]);
	 printf(" check 3\n"); 
	 dealloca_lista(risultato);
     }

     /*
      * Rimuovo gli eventi che hanno un match con pattern[]
      */
     
     partial = 0;
       for (k=0;pattern[k]!=NULL; k++) {
	 int cont=0;
	 database=rimuovi(pattern[k],database);
	 cont= conta(database);
	 
	 fprintf(stderr,"cont %d: suo %d\n", cont, i-partial-npatternmatch[k]);
	 CU_ASSERT_EQUAL_FATAL(cont, i-partial-npatternmatch[k]); 
	 partial +=npatternmatch[k];
     }
      fprintf(stderr," check 5\n");
      i-=partial;
     
    /*
     * Registro gli eventi rimasti 
     */
     
      printf("Registro l'agenda.\n");
     fsa=fopen(ftest,"w");
     CU_ASSERT_PTR_NOT_NULL_FATAL(fsa); 
	 printf("check 6\n");
     r=storeAgenda(fsa,database);
	 
	 fprintf(stderr,"check 7, ritornato : %d\n",r);
	 fflush(stderr); 
     
	 CU_ASSERT_EQUAL_FATAL(r, i);
     fclose(fsa);

	 
     /*
      * Verifico la scrittura
      */
     fsa=fopen(ftest,"r");
     CU_ASSERT_PTR_NOT_NULL_FATAL(fsa); 
     r=loadAgenda(fsa,&database1);     
     CU_ASSERT_EQUAL_FATAL(r, i);
     fclose(fsa);

	  printf("check 8\n");
     tmp=database;
     tmp1=database1;
     while (tmp!=NULL && tmp1!=NULL) {
	 printf("check 9\n");
	 CU_ASSERT_EQUAL_FATAL(strncmp(tmp->ptev->data,tmp1->ptev->data,LDATA+1), 0);
	 CU_ASSERT_EQUAL_FATAL(strncmp(tmp->ptev->utente,tmp1->ptev->utente,LUTENTE+1), 0);
	 CU_ASSERT_EQUAL_FATAL(strncmp(tmp->ptev->descrizione,tmp1->ptev->descrizione,LDESCRIZIONE+1), 0);
	 tmp=tmp->next;
	 tmp1=tmp1->next;
     }
     
     CU_ASSERT_EQUAL_FATAL(tmp,tmp1);
     printf(" Finito!\n");
     
}


/**
 * \test
 *
 * Testa il caricamento, loscaricamento e le operazioni sull'agenda in memoria
 */
void load_store_test (void)
{
    run_testloadstore(FILE_TEST1,FILE_TEST2);
}

/** 
 * Definizione della suite. 
 */
void add_suite_loadstoreagenda(void)
{
    CU_pSuite load_store_suite = NULL;

    load_store_suite = CU_add_suite("Load Store Test", success_init, success_clean);
    CU_add_test(load_store_suite, "Test Load e Store dei dati numero 2", load_store_test);

}

