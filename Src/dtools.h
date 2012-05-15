/**
 * \file
 * \author Francesco Cervigni
 * \brief agenda come lista di eventi
 *
 * Definisco il tipo agenda_t per gli elementi della lista di agende in memoria centrale.
 * Defininsco inoltre delle funzioni per la manipolazione di questa lista, queste funzioni 
 * sono versioni modificate di quelle per la gestione delle liste di elem_t definite in llist.h 
 * 
 */
/* TODO provvisorio */
#include "llist.h"
#include "lbase.h"

#define PROVA(AZIONE, RET) if(AZIONE==NULL) { perror("Riscontrato errore :"); return RET; }

#define MAXNOME 100

typedef struct agenda_t{
	
/** stringa contenente il nome del file, così come lo si può trovare nella directory */
char nome[MAXNOME];

/** puntatore alla lista di elem_t che rappresenta agenda */
elem_t * testa;

/** stringa contenente il nome del file, così come lo si può trovare nella directory */
struct agenda_t *next;

}agenda_t;

int add_agenda(agenda_t** agende, char * nomeag, elem_t* testa){
	/* appendo il nuovo elemento in testa */

	agenda_t *temp;
	agenda_t * newelem;
		if(*agende!=NULL)
			temp= *agende;
		else
			temp=NULL;


		PROVA(nomeag,-1)
		else {
				newelem = malloc (sizeof(agenda_t));
				newelem->testa=testa;
				
				strncpy(newelem->nome,nomeag,MAXNOME);
				
				newelem->next=temp;
				*agende=newelem;
  			}

		return 0;

}

elem_t *cerca_agenda (char *nomeag, agenda_t* agende){

	agenda_t *checkthis;
	checkthis=agende;
		
	if(strlen(nomeag)==0)
		return NULL;

	PROVA(agende,NULL)

		 do{
			#ifdef DEBUG 
			printf("analizzo %s \n", checkthis->nome);
			#endif	 
			if (strcmp(checkthis->nome,nomeag)==0)
				return checkthis->testa;

			checkthis=checkthis->next;

			}while (checkthis!=NULL);

	/* se non ha trova tovato l'agenda corrispondente'*/
	return NULL;

}

void dealloca_agende(agenda_t* lista){
	/* TODO, sembra andar bene..*/

		agenda_t *mem=lista;  /* variabile di appoggio per memorizzare l'elemento attuale */


		while(lista != NULL)
		{
			mem=lista;
			if(lista->testa != NULL)
				dealloca_lista(lista->testa); 
			
			lista=lista->next;
			if(mem != NULL)
				free(mem);
		}

	    if(lista!=NULL)
	    	free(lista);
		
		
		return;


}

