/*
 * llist2.c
 *
 *
 *        Author: frankie
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lbase.h"
#include "llist.h"
#include "errno.h"

#define DEBUG

#define PROVA(AZIONE, RET) if(AZIONE==NULL) { perror("Riscontrato errore :"); return RET; }

int add(elem_t** agenda, evento_t* ev){
	/* appendo il nuovo elemento in testa */

	elem_t *temp;
	elem_t * newelem;
		if(*agenda!=NULL)
			temp= *agenda;
		else
			temp=NULL;


		PROVA(ev,-1)
		else {
				newelem = malloc (sizeof(elem_t));
				newelem->ptev=malloc(sizeof(evento_t));
				memcpy(newelem->ptev,ev,sizeof(evento_t));
				newelem->next=temp;
				*agenda=newelem;
  			}

		return 0;

}

int cerca(char data[], elem_t* agenda, elem_t** trovati){

	int count=0;
	elem_t *checkthis;
	checkthis=agenda;
	
	/* lo inizializzo a NULL, per il caso in cui non ci siamo trovati, sarà più semplice per la deallocazione */
	*trovati=NULL;
	
	
	if(isdata(data)==-1)
		return -1;

	PROVA(agenda,-1)

		 do{
			#ifdef DEBUG 
			printf("analizzo %s \n", checkthis->ptev->data);
			#endif	 
			if (matchData(data,checkthis->ptev)==1)
				{
				add(trovati, checkthis->ptev);
				count++;
				}

			checkthis=checkthis->next;

			}while (checkthis!=NULL);

	return count;

}

elem_t* rimuovi(char pattern[], elem_t* agenda){
		elem_t *check;
		PROVA(agenda, NULL)

		if(matchPattern(pattern,agenda->ptev))
				{
				check=agenda;	
				agenda=agenda->next;
				}
				else
					check=agenda;  /* so già dall'if precedente che agenda punta a un evento esistente, che non è quello da cancellare */

		while(check->next != NULL)
			if(matchPattern(pattern,check->next->ptev))  /* i confronti avvengono, periò a due posizioni di distanza nella lista */
				check->next=check->next->next;
				else
				check=check->next; /* itero */

		return agenda;
}

int loadAgenda(FILE* ingresso, elem_t** agenda){
	elem_t *nuovoevento;
	char nuovalinea [LRECORD+1];
	int count = 0;

	
	PROVA(ingresso,-1)

	if (fgets(nuovalinea,LRECORD+2,ingresso) == NULL)
			{
			fprintf(stderr,"file %s vuoto", ingresso);
			return -1;
			}
	

	nuovoevento = malloc(sizeof(elem_t ));  /*preparo il prossimo elemento da aggiungere alla lista*/
	*agenda= nuovoevento;
	

	nuovoevento->ptev = convertiRecord(nuovalinea);
	nuovoevento->next = NULL;
	count++;
	
	

		while(fgets(nuovalinea,LRECORD+2,ingresso))
			{
			nuovoevento->next= malloc (sizeof(elem_t *));
			nuovoevento->next->ptev=convertiRecord(nuovalinea);
		    nuovoevento->next->next=NULL;
			nuovoevento= nuovoevento->next;
		    count++;
		    }
		
	return count;

}

int storeAgenda(FILE* uscita, elem_t* agenda){

	int count=0;
	char ev_stringa [LRECORD+1];
	
		while (agenda!=NULL)
		{
			
			convertiEvento(agenda->ptev,ev_stringa);
			#ifdef DEBUG
			printf("scrivo : %s",ev_stringa);
			#endif
			if(fputs(ev_stringa,uscita)==EOF)
				return -1;
			
			agenda=agenda->next;
			
			count++;
		}

		return count;

}

void dealloca_lista(elem_t* lista){
	/* uso il puntatore lista come appoggio per memorizzare l'indirizzo dell'evento attuale ad ogni ciclo */


		elem_t *mem=lista;  /* variabile di appoggio per memorizzare l'elemento attuale */


		while(lista != NULL)
		{
			mem=lista;
			if(lista->ptev != NULL)
				free(lista->ptev); 
			
			lista=lista->next;
			if(mem != NULL)
				free(mem);
		}

	    if(lista!=NULL)
	    	free(lista);
		
		
		return;


}
