/*
 * provalist.c
 *
 *  Created on: 2-mag-2009
 *      Author: frankie
 */

#include "lbase.h"
#include "llist.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mcheck.h>

int main(){

	mtrace();
char *prova;
prova = calloc(80, sizeof(char));
strcpy(prova,"12-01-2008 ciaociao#fefefadesaddsf");

evento_t *e1;
e1=convertiRecord(prova);

printf("%s\n",e1->data);
printf("%s\n",e1->utente);
printf("%s\n",e1->descrizione);



if (matchData("12-01-2008 alkacoa#alcjaocauo",e1))
  printf("primo match ok  \n");
if (matchData("**-01-2008 alkacoa#alcjaocauo",e1))
  printf("secondo match ok\n");

char *patt;
patt="ciao";

if (matchPattern(patt,e1))
  printf("terzo match ok\n");

elem_t** trov=malloc(sizeof(elem_t *));



/* proviamo la add  */

printf("Proviamo la add..\n");

elem_t ** myagenda=malloc(sizeof(elem_t *));

if(add(myagenda,e1)==0)
	printf("add ok\n");

elem_t *app= *myagenda;



printf("%s \n", app->ptev->descrizione);

if (cerca("12-01-2008",app,trov))
  printf("cerca ok\n");

FILE *prova1 = fopen("prova1.txt","w");

if(storeAgenda(prova1,app)>0)
	printf("store  ok\n");

fflush(stdout);
elem_t** vediamo =malloc(sizeof(elem_t *));

FILE *prova2= fopen("appoggio.txt","r");

fflush(prova1);

if(loadAgenda(prova2,vediamo)>0)
	printf("load  ok\n");

/*printf("%s",(*vediamo)->ptev->data); */

if(rimuovi("adfa", *vediamo))
	printf("remove  ok\n");
	
printf("ora le provo tutte :\n");

char *v="12-03-2008";
int a=isdata2(v);

printf("check");
fflush(stdout);
printf("%s %d","12-03-2008",isdata2("12-03-2008"));
printf("%s %d","**-03-2008",isdata2("**-03-2008"));
printf("%s %d","12-*-2008",isdata2("12-*-2008"));



	
	

dealloca_lista(*vediamo);

	return 0;
}

