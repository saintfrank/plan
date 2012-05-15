/*
 * lbase.c
 *
 *  Created on: 9-mag-2009
 *      Author: frankie
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lbase.h"
#include "errno.h"
#include <ctype.h>


#define PROVA(AZIONE, RET) if(AZIONE==NULL) { perror("Riscontrato errore :"); return RET; }

int isdata(char data[]){
	
	char *datapointer=calloc(LDATA,sizeof(char));
	char *giorno,*mese,*anno, *end;	
	
	if(strlen(data)<10)
		return -1;
		

	/* per rendere strtok più sicura */	
	strcpy(datapointer,data);
	
	/* controllo la lunghezza*/
	if(strlen(datapointer)<10)
		return -1;
		
	giorno=strtok(datapointer,"-");
	mese=strtok(NULL,"-");
	anno=strtok(NULL,"");	
	
	PROVA(data,-1)

    /* controllo che il giorno sia un numero o "**" e che mese e anno sia dei numeri validi */
	if((!((strtol(giorno,&end,10)!=0) || (strcmp("**",giorno) ==0) )) || (strtol(mese,&end,10)==0) || (strtol(anno,&end,10)==0) )
		{
		free(datapointer);	
		return -1;
		}	
	
	free(datapointer);
	return 0;

}

/** Trasforma un record di tipo evento su file
    es: 01-02-2008 davide\#Bollo auto
    in una struttura di tipo evento
  \param  r  la stringa che rappresenta l'evento

  \retval p (puntatore al nuovo evento) se la conversione e' andata a buon fine e non si sono verificati errori
  \retval NULL se si e' verificato un errore (setta errno)
 */
evento_t* convertiRecord(char r[]){

	evento_t* new=malloc(sizeof(evento_t));

   char *app1, *app2, *app3;



   PROVA((app1=strtok(r," #")),NULL);
   strcpy(new->data,app1);
   PROVA((app2=strtok(NULL,"#")),NULL);
   strcpy(new->utente,app2);
   PROVA((app3=strtok(NULL,"\0")),NULL);
   strcpy(new->descrizione,app3);

   return new;
}



/** Trasforma una struttura di tipo evento in una stringa r nel formato record tipo evento su file
    es: 01-02-2008 davide\#Bollo auto
  \param r  la stringa da riempire (conterra' il record secondo il formato)
  \param e la struttura evento da convertire

   \retval  0 se tutto e' andato a buon fine
   \retval -1 altrimenti
 */
int convertiEvento(evento_t* e,char r[]){

	char *new=calloc(LRECORD+1,sizeof(char));
				
	PROVA(e,-1)
	
	sprintf(new,"%s %s#%s",e->data,e->utente,e->descrizione);	
				
	strcpy(r,new);

	free(new);
	
	return 0;
		
}

/**
 Controlla se un'evento ha data corrispondente a quella contenuta in una stringa il formato della stringa puo' essere:
   `gg-mm-aaaa' (es `12-12-1998') oppure
   `**-mm-aaaa' (es `**-12-1998') nel secondo caso il matching e' positivo per tutti gli eventi del mese considerato

 \param b  la data da cercare in formato stringa
 \param e l'evento da verificare
 \retval 1 se c'e' un matching fra data ed evento
 \retval 0 se NON c'e' un matching fra data ed evento
 \retval -1 se si e' verificato un errore (es data mal formattata)
 */
int matchData(char b[],evento_t* e){

	char *datadib;
	int rit=-1;

	PROVA(e,-1)
	PROVA(b,-1)
	
	if(isdata(b)==-1)
		{
		#ifdef DEBUG
		fprintf(stderr,"data non valida!");	
		#endif
		return -1;
	    }

		/* la strtok non lavora con allocazioni di memoria non modificabili, quindi ho utilizzato na memoria di appoggio per motivi di generalità*/
		
		
	    datadib=calloc(LDATA + 1,sizeof(char));
		strcpy(datadib,b);
		datadib=strtok (datadib," ");
	
		rit=( ( strcmp(datadib,e->data)==0 ) || (  datadib[0]=='*' && datadib[1]=='*'  && (strcmp( (datadib+3)  ,( e->data+3 ) )==0 )) );

		free(datadib);
	
		return rit;
}


/**
 Controlla se uno dei campi dell'evento contiene un pattern specificato

 \param p  il pattern da cercare (p==NULL da sempre match positivo)
 \param e l'evento da verificare

 \retval 1 se c'e' un matching fra pattern ed evento
 \retval 0 se NON c'e' un matching
 \retval -1 se si e' verificato un errore
 */
int matchPattern(char p[],evento_t* e){
	PROVA(e,-1)

	PROVA(p,1)

	return ((strstr(e->data,p)!=NULL) || (strstr(e->descrizione,p)!=NULL) || strstr(e->utente,p)!=NULL);


}

