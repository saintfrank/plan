#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include "lcscom.h"
#include "lbase.h"
#include "llist.h"
#include "dtools.h"

#define true 1
#define DEBUG_CO
#define DEBUG
#define SOCKETNAME "./tmp/dsock"
#define LIBERA  fprintf(stderr,"inizio le free. ");free(appoggio); free(new); free(agenda); free(lista_ritornata); free(data); free(pattern); free(nome_agenda); fprintf(stderr,"fine free. ");
#define MAXMESSAGEBUFFER 125
#define UNIX_PATH_MAX   108

void *personal_fun(void *fd);
int name_to_agenda(char *nome, elem_t **agenda_ritorno );
int name_to_store(char *nome, elem_t *agenda );
int esegui(char azione, char* stringa,char** errore);
int to_normal_string(char* ingresso, int lunghezza,char **new);
int is_file_regolare(const struct dirent *file);

/* variablili per il File System*/
char *diragende;
DIR * wd;
struct dirent *temp_entry;
elem_t **ag_list;
agenda_t *agende=NULL;

int main(int argc, char *argv[]){
	 
	serverChannel_t server;
	channel_t new_connection;
	pthread_t new_thread;
	FILE *file_att = NULL;
	elem_t *newAgenda;
	char *path_completo = calloc(UNIX_PATH_MAX,sizeof(char));
	
	int err = 0;
	int filenum =0;
	struct dirent **file_letti;
	int i,ag_ins,elem_letti=0;
	
	
	
	
	#ifdef DEBUG
	fprintf(stderr,"prova %d %s %s\n",argc, argv[0],argv[1]);
	fflush(stderr);
	#endif
	
	if(argc > 1 && argv[1]!=NULL)
		{
		/* TODO devo mettere una macro con un massimo di lunghezza, non è il massimo della sicurezza .. */	
		diragende = calloc(strlen(argv[1])+3,sizeof(char));
		sprintf(diragende,"./%s/", argv[1]);
		}else{
			fprintf(stderr,"cartella non inserita ! ");
			fflush(stderr);		
			return -1;	
			}
	
	/* opendir, se non trova la cartella, la crea */	
	if((wd = opendir(argv[1])) == NULL)
		{
			perror("problemi con l'apertura della cartella '");
			return -1;
		}
		
	if ((filenum = scandir(argv[1], &file_letti,is_file_regolare,  NULL)) == -1)
			{
				perror ("errore con la scandir :");
				return -1;
				
			}
			else{
				
				fprintf(stderr,"scandir ha letto %d files \n",filenum);
				
				for(i=0;i<filenum; i++)
					{
						fprintf(stderr,"scandir ha letto il file %s %s\n",diragende,file_letti[i]->d_name);
						fprintf(stderr,"prima    di fopen \n");
						
						
						fprintf(stderr,"diragende è %s\n",diragende);
						
						sprintf(path_completo,"%s%s",diragende,file_letti[i]->d_name);	
						
						fprintf(stderr,"diragende è %s\n",diragende);
						
					    fprintf(stderr,"il path completo è %s\n",path_completo);
						
						if((file_att=fopen(path_completo,"r")) == NULL)
							{
								fprintf(stderr,"dopo di fopen\n");
								fprintf(stderr,"problemi ad aprire il file %s: \n",file_letti[i]->d_name);
								perror("Problemi nell'apertura di : ");	
								return -1;
							}
						fprintf(stderr,"ho aperto il file %s: \n",file_letti[i]->d_name);
						elem_letti = loadAgenda(file_att,&newAgenda);
						fprintf(stderr,"elementi letti da %s : %d: \n",file_letti[i]->d_name, elem_letti);
						if((ag_ins =add_agenda(&agende,file_letti[i]->d_name,newAgenda)) == 0)
						{
							#ifdef DEBUG
							fprintf(stderr,"problemi con l'inserimento agende :%d \n",ag_ins);
							#endif
						}
						#ifdef DEBUG
						fprintf(stderr,"%d agenda inserita \n ",ag_ins);
						#endif		
					}
				
				}
			
						
    
				
		/*aggiungi_agenda(temp_entry->d_name);*/
      (void) closedir (wd);
    
  


	
	/* TODO ora dovrei esplorare la cartella */
	
	server = createServerChannel(SOCKETNAME);
	
	#ifdef DEBUG_COM
	fprintf(stderr,"STEP createserver ha ritornato %d\n",server);
	#endif
	
	if( server == -1) 
		{
		 perror("creazione :");	
		 #ifdef DEBUG_COM
		 fprintf(stderr, "problemi con la creazione del server \n");
		 #endif
		 return -1;
		} 
	 
	 
	while(true){	 
	 
		#ifdef DEBUG_COM
		fprintf(stderr,"SERVER - Attendo la accept .. \n");
		fflush(stderr);
		#endif
	 
		if((int)(new_connection = acceptConnection(server)) == -1)
			{
			fprintf(stderr, "problemi con la accept, newconnection %d \n", (int)new_connection);
			return -1;
			}
			 
		
		#ifdef DEBUG_COM
		fprintf(stderr,"SERVER - accept andata! \n");
		#endif
		
		if( (err = pthread_create(&new_thread,NULL,&personal_fun,(channel_t *)&new_connection)) != 0 )
									 {
									 perror("pthread_create :")	;
									 printf("SERVER -errore nella pthread_create !  err: %d \n",err);
									 return -1;									 
									 }
									 
		#ifdef DEBUG_COM
		fprintf(stderr,"SERVER		pthread create andata ! !\n");
		#endif												 
		
		}
		
		closeSocket(server);
		#ifdef DEBUG_COM
		fprintf(stderr,"SERVER	Ho chiuso il  server socket %d\n",(int)server);
		#endif
		
		return 0;
		
}


void *personal_fun(void *fd)
 {
    channel_t *canale = (channel_t *)fd;
	message_t *richiesta=malloc(sizeof(message_t));
	message_t *risposta=malloc(sizeof(message_t));	
	
	int ricevuti = 0;
	int inviati = 0;
	int esito;
		
	char *errore=calloc(200,sizeof(char));
	
	
	#ifdef DEBUG
	fprintf(stderr,"PERSONAL	inizio routine %d !!!\n", (int)*canale);
	#endif		

						
		if( (ricevuti = receiveMessage(*canale, richiesta)) <= 0 )
			{
			perror("ricezione");	
			fprintf(stderr, "problemi con la ricezione \n");
			pthread_exit((void *) -1 );
			} 
		
		fprintf(stderr,"receive ok ");
		
		#ifdef DEBUG
		if(richiesta->length > 0)
				{				       
				char * stringa_convertita = calloc(MAXMESSAGEBUFFER,sizeof(char)); /* da usare con la funzione to_normal_string() */
				to_normal_string(richiesta->buffer,richiesta->length,&stringa_convertita);			
				fprintf(stderr,"PERSONAL - ho ticevuto  %s !!!\n", stringa_convertita);
				free(stringa_convertita);
				}
		#endif		
			
		/* effettuo tutte le operazioni */	
		
		esito = esegui(richiesta->type, richiesta->buffer, &errore );
			
		
		#ifdef DEBUG
		fprintf(stderr,"PERSONAL - l'esito è stato %d - errore (a capo):\n %s \n", esito,errore);
		#endif		
		
		risposta->type=(esito==0)?MSG_OK:MSG_ERROR;
		risposta->length=0;  /* TODO temporanea */
		risposta->buffer= errore;
			
		if( (inviati = sendMessage(*canale, risposta)) <= 0 )
			{
			perror("invio");	
			#ifdef DEBUG
			fprintf(stderr, "problemi con l'invio della risposta \n");
			#endif
			pthread_exit((void *) -1 );
			} 

	
	free(richiesta);
	free(risposta);
	free(errore);
    closeSocket(*canale) ;
	#ifdef DEBUG
	fprintf(stderr,"PERSONAL	Ho chiuso il socket %d\n",(int)*canale);
	#endif
	
	pthread_exit((void *)0);
   
}

int esegui(char azione, char* stringa,char** errore){
	
	
	/* variabili utili all'interno dei case */
	int risultato;
	char *nome_agenda=calloc(MAXMESSAGEBUFFER,sizeof(char));
	char *data=calloc(MAXMESSAGEBUFFER,sizeof(char));
	char *pattern=calloc(MAXMESSAGEBUFFER,sizeof(char));
	char *appoggio=calloc(MAXMESSAGEBUFFER,sizeof(char));   /* variabile d'appoggio'*/
	
	evento_t *new= malloc(sizeof(evento_t));
	
	elem_t *agenda=malloc(sizeof(elem_t));
	elem_t *lista_ritornata=malloc(sizeof(elem_t ));
	
	#ifdef DEBUG
	fprintf(stderr,"ESEGUI	Inizio - I parametri passati sono %c - %s \n",azione, stringa);
	#endif
	
	if(stringa == NULL)
		return -1;
	
	switch (azione)
		{
			case MSG_MKAGENDA:
						
				if( strcpy(nome_agenda,stringa) == NULL)
							{
							#ifdef DEBUG
							fprintf(stderr ,"ESEGUI	stringa non valida, agenda non trovata ! !\n");
							#endif
							LIBERA	
							return -1;
							}
							
				#ifdef DEBUG
				sprintf(*errore,"ESEGUI	strcpy ok  ! !\n");
				#endif			
				
				
				fprintf(stderr," dir = %s nome: %s \n", diragende,nome_agenda);						
				sprintf(nome_agenda,"%s%s",diragende,stringa);	
					
				if(cerca_agenda(nome_agenda,agende) != NULL)
					{
								#ifdef DEBUG   
								fprintf(stderr,"ESEGUI  Percorso sbagliato : %s \n",nome_agenda);
								#endif
								
								
								/* TODO stampo l'errore  in stdout */
								/*if (perror = FILE PRESENT)*/
								sprintf(*errore,"dplan: %s: Cannot create, agenda already present ",stringa);
								/*else
									sprintf(*errore,"dplan: %s: Cannot create",stringa);*/
								
								LIBERA
								return -1;
														
						}else{
									
							add_agenda(&agende,stringa,NULL);
							
							#ifdef DEBUG   
							fprintf(stderr,"ESEGUI  ho inserito alla lista delle agende : %s \n",stringa);
							#endif
																					
							}
							
				#ifdef DEBUG
				fprintf(stderr,"ESEGUI	fopen ok  ! !\n");
				#endif
				
				/* stampo la comferma in stdout */
				sprintf(*errore,"dplan: %s: Created",stringa);
							
				LIBERA
				return 0;
			
						
			case MSG_RMAGENDA:
			
				if( strcpy(nome_agenda,stringa) == NULL)
							{	
							#ifdef DEBUG
							fprintf(stderr,"ESEGUI	stringa non valida, agenda è null ! !\n");
							#endif	
							LIBERA	
							return -1;
							}						
							
				sprintf(nome_agenda,"%s%s",diragende,stringa);	
											
				if (remove(nome_agenda)!= 0)
							   {
								#ifdef DEBUG
								perror("ESEGUI	Percorso sbagliato nella remove :");
								#endif
								
								
								/* TODO devo controllare errno */
								/* if(file non vuoto) */
								sprintf(*errore,"dplan: %s: Agenda not empty, cannot remove",stringa);
								/*else
									sprintf(*errore,"dplan: %s: Errors with remove",stringa); */
								
								LIBERA	
								return -1;
								}
				
				/* stampo la comferma in stdout */
				sprintf(*errore,"dplan: %s: Removed",stringa);
								
								
				LIBERA	
				return 0;
						
						
			case MSG_INSERT:	
					
				
				if(strcpy(nome_agenda,stringa) == NULL)
							{	
							#ifdef DEBUG
							fprintf(stderr,"ESEGUI	stringa non valida, agenda non trovata ! !\n");
							#endif	
							LIBERA	
							return -1;
							}
					
				if( strcpy( new->data, (stringa+= (strlen(stringa)+1) ) ) == NULL)
						{	
							#ifdef DEBUG
							fprintf(stderr,"ESEGUI	stringa non valida, data non riconosciuta ! !\n");
							#endif	
							LIBERA	
							return -1;
							}
					
				if( strcpy(new->utente,(stringa+=(strlen(stringa)+1))) == NULL)
						{	
							#ifdef DEBUG
							fprintf(stderr,"ESEGUI	stringa non valida, utente non riconosciuto ! !\n");
							#endif	
							LIBERA	
							return -1;
							}
							
				if( strcpy(new->descrizione,(stringa+=(strlen(stringa)+1)))  == NULL)
						{	
							#ifdef DEBUG
							fprintf(stderr,"ESEGUI	stringa non valida, descrizione non riconosciuta ! !\n");
							#endif	
							LIBERA	
							return -1;
							}		
				/* TODO  controllare questa, ceh non scriva due volte il path*/	
				sprintf(nome_agenda,"%s%s",diragende,nome_agenda);	
									
				if(name_to_agenda(nome_agenda,&agenda) == -1)
							{	
							#ifdef DEBUG
							fprintf(stderr,"ESEGUI	l'apertura dell'agenda dal path -%s- ha fallito'' ! !\n",nome_agenda);
							#endif	
							LIBERA	
							return -1;
							}	
					
					if( (-add(&agenda, new)) == -1)	/* nego il risultato per consistenza, add ritorna 1 in caso di errore, io voglio ritornare -1*/
							{ 
							#ifdef DEBUG
							fprintf(stderr,"ESEGUI	problemi con la add' ! !\n");
							#endif	
							LIBERA	
							return -1;
							}   
										
					if( name_to_store(nome_agenda,agenda) == -1 )
							{	
							#ifdef DEBUG
							fprintf(stderr,"ESEGUI	problemi con la scrittura dell'agenda aggiornata nel file' ! !\n");
							#endif	
							LIBERA	
							return -1;
							}
					
					LIBERA
					return 0;
				
				
			case MSG_RMPATTERN:
				
					if(strcpy(nome_agenda,stringa) == NULL)
						{	
							#ifdef DEBUG
							fprintf(stderr,"ESEGUI	stringa non valida, agenda non trovata ! !\n");
							#endif	
							LIBERA	
							return -1;
							}
					
					if( strcpy( pattern ,( stringa += ( strlen(stringa)+1) ) )  == NULL)
							{	
							#ifdef DEBUG
							fprintf(stderr,"ESEGUI	stringa non valida, pattern non trovata ! !\n");
							#endif	
							
							sprintf(*errore ,"stringa non valida, pattern non trovato ! !\n");
							
							LIBERA	
							return -1;
							}
							
				if(name_to_agenda(nome_agenda,&agenda) == -1)
							{	
							#ifdef DEBUG
							perror("ESEGUI	l'apertura dell'agenda ha fallito'' ! !\n");
							#endif	
							sprintf(*errore,"problemi con l'apertura dall'agenda");
							
							LIBERA	
							return -1;
							}			
						
				lista_ritornata = rimuovi(pattern, agenda);
				
				if( name_to_store(nome_agenda,lista_ritornata) == -1 )
							{	
							#ifdef DEBUG
							fprintf(stderr,"ESEGUI	problemi con la scrittura dell'agenda aggiornata nel file' ! !\n");
							#endif	
							LIBERA	
							return -1;
							}	
				
				LIBERA	
				return 0;
			
			
			case (MSG_EMESE ||MSG_EGIORNO):
						
				
				if( strcpy(nome_agenda,stringa) == NULL)
					{	
						#ifdef DEBUG
						fprintf(stderr,"ESEGUI	stringa non valida, on trovata ! !\n");
						#endif	
						LIBERA	
						return -1;
						}
				
				if( strcpy( data , ( stringa += ( strlen(stringa)+1) ) ) == NULL)
						{	
						#ifdef DEBUG
						fprintf(stderr,"ESEGUI	stringa non valida, mese non tronvato!!! \n");
						#endif	
						LIBERA	
						return -1;
						}
						
				if(name_to_agenda(nome_agenda,&agenda) == -1)
							{	
							#ifdef DEBUG
							fprintf(stderr,"ESEGUI	l'apertura dell'agenda ha fallito'' ! !\n");
							#endif	
							LIBERA	
							return -1;
							}			
						
				if((risultato = cerca(data, agenda, &lista_ritornata)) == -1 )
						   {	
							#ifdef DEBUG
							fprintf(stderr,"ESEGUI	la funzione cerca ha fallito \n");
							#endif	
							LIBERA	
							return -1;
							}else{
								  #ifdef DEBUG
								  fprintf(stderr,"ESEGUI	trovati da cerca %d elementi \n",risultato);
								  #endif								
								}
				
				
				if( storeAgenda(stdout,lista_ritornata) == -1 )
							{	
							#ifdef DEBUG
							fprintf(stderr,"ESEGUI	problemi con la scrittura su output dei risultati ! !\n");
							#endif	
							LIBERA	
							return -1;
							}	
								
							
				LIBERA	
				return 0;
			
			case '?':
				{
				 #ifdef DEBUG
				 fprintf(stderr,"ESEGUI	carattere non riconosciuto \n");
				 #endif								
				}
				LIBERA	
				return 1;
		
			default:
				LIBERA	
				abort ();
					
			}
			
	
	
}



/* restituisce l'agenda caricata dal File corrispondente alla stringa path
 * 
 * 
 * @param 		path è il path del file, agenda_ritorno è il puntatore a puntatore usato per ritornare indirettamente l'agenda caricata
 * @modifies	agenda_ritorno punterà all'agenda caricata in caso di successo'
 * @return		0 zero in caso di successo, -1 in caso di errore 
 */

int name_to_agenda(char *nome, elem_t **agenda_ritorno ){
	
	FILE *file;
	int risultato;
	
	if(nome==NULL)
		{
		#ifdef DEBUG
		fprintf(stderr,"Percorso è NULL : %s",nome);
		#endif
		return -1;
	    }
	
	if ((file = fopen(nome, "r"))==NULL)
	               {
					#ifdef DEBUG
					fprintf(stderr,"Percorso sbagliato : %s",nome);
			        #endif
					return -1;
	    	        }
	
	risultato = loadAgenda(file, agenda_ritorno);
	/* TODO vedere se agenda deve essere inizializzata */
		
	fclose(file);	
		
	return risultato;
	
	}
	
	
/* scrive agenda nel file contrassegnato da path
 * 
 * 
 * @param 		path è il path del file, agenda è il puntatore all'agenda da salvare, se esiste, viene sovraiscritta
 * 
 * @return		0 zero in caso di successo, -1 in caso di errore 
 */
int name_to_store(char *nome, elem_t *agenda ){
	
	FILE *file;
	int risultato;
	
	if(nome==NULL)
		{
		#ifdef DEBUG
		fprintf(stderr,"Percorso è NULL : %s",nome);
		#endif
		return -1;
	    }
	
	if ((file = fopen(nome, "w+"))==NULL)
	               {
					#ifdef DEBUG
					fprintf(stderr,"Percorso sbagliato : %s",nome);
			        #endif
					return -1;
	    	        }
	
	risultato = storeAgenda(file, agenda);
	/* TODO vedere se agenda deve essere inizializzata */
		
	fclose(file);	
		
	return risultato;
	
	}
	
/* piccola funzione per le stampe di debug, oltre a convertire la stringa CONTA il numero di tokens separati da \0
 * 
 * @param  		ingresso è una stringa della forma ccccc\0yyyy\0fffff\0, lunghezza è il numero dai caratteri significativi
 * @modifies 	new: punterà a una stringa formata dalla stringa ingresso privata dei \0, quindi cccccyyyyyfffff
 * @return 		il numero di tokens trovati
 */
	
int to_normal_string(char* ingresso, int lunghezza,char **new){
		int app=lunghezza;
		
		int tokens=1; /*  a questa funzione faccio anceh calcolare quanti tokens ci sono*/			
		
		if(ingresso == NULL || *new==NULL || (lunghezza == 0))
			return 0;
			
		app -= strlen(ingresso);
		strcpy(*new,ingresso);	
		
		do{
						
			sprintf(*new,"%s%s",*new,( ingresso += ( strlen(ingresso)+1) ) );
			app -= strlen(ingresso);
						
			tokens++;
			
			}while(app>0 && (strlen(ingresso)>0) && (ingresso != NULL));
		
		sprintf(*new,"%s%c",*new,'\0');
			
		return tokens;
		
		}
		
/* sarà la mia funzione "selector" usata per scandir */		
int is_file_regolare(const struct dirent *file){
		#ifdef DEBUG
		fprintf(stderr,"analizzo il file %s, di tipo %d con %d \n",file->d_name,file->d_type,DT_REG);
		#endif
		return (file->d_type == DT_REG);
		
	}
	

