#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#define true 1
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include "lbase.c"
#include "lcscom.h"

#define DEBU
#define DEBUG_CO

#define SOCKETNAME "./tmp/dsock"

#define MAXMESSAGEBUFFER 125

int parse(int argc, char **argv, char *msg, char ** buffer,int *lunghezza);

int to_normal_string(char* ingresso, int lunghezza,char **new);

int main (int argc, char **argv){ /*  gli passo l'indirizzo del dispatcher */

	channel_t server;
	int risultato_parsing ;
	int ninviati = 0;
	int nricevuti = 0;
	int *lunghezza;
	char messaggio;
	char *buffer= calloc(MAXMESSAGEBUFFER,sizeof(char)) ;
	message_t *new_message = malloc(sizeof(message_t)) ;
	message_t *risposta = malloc(sizeof(message_t)) ;
	char *appoggio=calloc(MAXMESSAGEBUFFER,sizeof(char));;
	
	new_message->buffer=calloc(MAXMESSAGEBUFFER,sizeof(char));
			
	*lunghezza=0;					
			
	if( (risultato_parsing = parse(argc, argv, &messaggio, &buffer, lunghezza)) == -1)
		{
		 #ifdef DEBUG	
		 fprintf(stderr, "problemi col parsing %d \n", (int)risultato_parsing);
		 #endif
		 return -1;
		} 
			
		#ifdef DEBUG
		if(risultato_parsing != -1)
			{	
			/*appoggio=  soltanto per debug, per utilizzar*/
			to_normal_string(buffer,*lunghezza,&appoggio);	
			fprintf(stderr, "parse ha ritornato %c, buffer %s, lunghezza: %d \n",(char) risultato_parsing, appoggio,*lunghezza);
			
			}
		
		#endif

	free(appoggio);
	
	new_message->type=messaggio;
	new_message->length=*lunghezza;
	memcpy(new_message->buffer,buffer,MAXMESSAGEBUFFER);

	server = openConnection(SOCKETNAME);

	if( server == -1) 
		{
		 fprintf(stderr, "problemi con la connessione al server \n");
		 return -1;
		}
		
	if( server == SOCKNAMETOOLONG )
		{
		 fprintf(stderr, "path troppo lungo \n");
		 return -1;
		}
		
		
	ninviati = sendMessage(server, new_message);
			
	if( ninviati == 0 )
		{
		 fprintf(stderr, "problemi nell'esecuzione della send \n");
		 return -1;
		}
	#ifdef DEBUG_COM
	fprintf(stderr,"	CLIENT ha inviato: %c, %d , %s\n",new_message->type, new_message->length,new_message->buffer);
	#endif
	
		
	nricevuti = receiveMessage(server, risposta);
			
		
	if( nricevuti == 0 )
		{
		 fprintf(stderr, "problemi nell'esecuzione della receive \n");
		 return -1;
		}
	
	#ifdef DEBUG_COM
	fprintf(stderr,"	CLIENT ricevuta risposta: %c, %d, %s\n",risposta-> type, risposta-> length, risposta->buffer);
	#endif
	
	fprintf(stdout,"risposta : %s\n",risposta->buffer);
	
	closeConnection(server);
	
	return 0;
}


/* parse effettua il parsing dei comandi , analizzandone le opzioni, e restituisce il messaggio per il server in caso positivo
 * 
 *  
 * @param  argc e argv sono gli stessi parametri del main
 * 		   msg è un puntatore a carattere che rappresenta il messaggio, buffer è una stringa usata per restituire il comando richiesto
 * @return 0 in caso di successo, -1 in caso di errore ( stampa su stderr eventuali messaggi di errore ) 
 */

int parse(int argc, char **argv, char *msg, char ** buffer, int *lunghezza)
{

  int cflag = 0;
  int gflag = 0;
  int dflag = 0;
  int uflag = 0;
  int mflag = 0;
  int rflag = 0;
  int qflag = 0;
  int tot_flags = 0;
  char *cvalue = "";
  char *gvalue = "";
  char *dvalue = "";
  char *uvalue = "";
  char *mvalue = "";
  char *rvalue = "";
  char *qvalue = "";


  int index;
  int c;
  int agendemul = 0;
  char *errore=calloc(100,sizeof(char));
  char *utente,*descrizione;

  opterr = 0;

  while ((c = getopt (argc, argv, "c:q:d:u:g:m:r:")) != -1)
    switch (c)
      {
      case 'c':
		cflag = 1;
		tot_flags++;
		if (strlen (optarg) > 20)
		  {
		   fprintf(stdout, "dplan: %s: Agenda max 20 caratteri\n",optarg);	
		   return -1;
		  }else
			  {
				cvalue = optarg;
				break;
			  }

      case 'q':
		/* TODO su q devo controllare che sia una agenda vuota per rimuovere */
		qflag = 1;
		tot_flags++;
		qvalue = optarg;
		break;
      case 'd':
		dflag = 1;
		tot_flags++;
		if(isdata(optarg)==0) 
		  {
		    dvalue = optarg;
		    break;
		  }else {
				fprintf(stdout, "dplan: %s: Data non corretta\n",optarg);	
				return -1;
				}

      case 'u':
		uflag = 1;
		tot_flags++;
		
		    uvalue = optarg;  /* evito di fare una malloc */
						
			utente=strtok(optarg,"#");
			
	
			
			/* analizzare l'utente */
			if(utente==NULL )
					{
					sprintf(errore,"dplan: %s: Formato evento non corretto (manca #)\n",utente);	
					break;
					}
				
			if((strlen(utente) > 8))
					{
					sprintf(errore, "dplan: %s: Formato evento non corretto (Utente max 8 caratteri)\n",utente);	
					break;
					}
			
			 #ifdef DEBUG
			fprintf(stderr, "utente %s \n",utente);
			#endif
			
			descrizione=strtok(NULL,"#");
			
			
			 
			
			#ifdef DEBUG
			fprintf(stderr, "descrizione è : %s \n",descrizione);
			#endif
			
			if(descrizione==NULL )
					{
					sprintf(errore,"dplan: %s: Formato evento non corretto (manca #)\n",utente);	
					break;
					}					
					
			if(strlen(descrizione)>80)		
					{
					sprintf(errore, "dplan: %s#%s: Formato evento non corretto (Descrizione max 80 caratteri)\n",utente,descrizione);	
					break;
					}
			
			sprintf(uvalue,"%s%c%s",utente,'\0',descrizione);	
													
		    break;
		  
      case 'g':
		gflag = 1;
		tot_flags++;
		if(isdata(optarg)==0) 
		  {
		    gvalue = optarg;
		    break;
		  }else{
				sprintf(errore, "dplan: %s: Data non corretta\n",optarg);	
				break;
				}
      case 'm':
		gflag = 1;
		tot_flags++;
		
		if(isdata(optarg)==0 && ( strlen(optarg) ==7 )) 
		  {
			gvalue = optarg;
		    break;
		  }else{
				sprintf(errore, "dplan: %s: Data non corretta\n",optarg);	
				break;
				}
      case 'r':
		rflag = 1;
		tot_flags++;
		rvalue = optarg;
		break;
      case '?':
		{
		  if (optopt == 'c')
		    fprintf (stdout, "dplan: opzione -%c richede argomento\n", optopt);
		  else if (isprint (optopt))
		    fprintf (stdout, "dplan: -%c: Opzione errata\n", optopt);
		  else
		    fprintf (stdout,
			     "carattere opzione sconosciuto `\\x%x'.\n", optopt);
		  return -1;
		}
		return 1;
      default:
	abort ();
      }

  /* il programma restituisce errore se:
   *      - nessuna opzione è passata     
   *      - solo -d o solo -u è passata
   *  	  - se ci sono due opzioni specificate e non sono esattamente -d e -u 
   *      - se più di due opzioni sono passate
   * 	  - se -g o -m o -r sono specificate ma non è specificata l'agenda
   * 	  - -g -m -r devo nessere specificate in mutua esclusione
   */


  #ifdef DEBUG
  fprintf(stderr, "CHECKPOINT 0 \n");
  #endif

  if (tot_flags == 0 || (tot_flags == 1 && (dflag == 1 || uflag == 1)) ||
      (tot_flags == 2 && ((dflag == 0 || uflag == 0))) || tot_flags > 2 ||
	  ((gflag ==1 ||mflag ==1 || rflag==1) && argv[optind]==NULL)||
	  (gflag+mflag+rflag > 1))
			{
			#ifdef DEBUG	
			printf("cflag = %d, cvalue  = %s,qflag = %d, qvalue  = %s,  gflag = %d, gvalue  = %s ,dflag = %d, dvalue  = %s ,\n uflag = %d, uvalue  = %s, mflag = %d, mvalue  = %s, rflag = %d, rvalue  = %s, tot_flags %d, agenda %s \n",
	 cflag, cvalue, qflag, qvalue, gflag, gvalue, dflag, dvalue, uflag,
	 uvalue, mflag, mvalue, rflag, rvalue, tot_flags,argv[optind]);	
			#endif
			fprintf(stdout, "dplan: Formato scorretto del comando\n");
			fflush(stdout);	
			return -1;
			}
	  
  if (strlen(errore))
			{
			fprintf(stdout, "%s", errore);	
			fflush(stdout);
			return -1;
			}
  
  #ifdef DEBUG
  fprintf(stderr, "CHECKPOINT 1 \n");
    
  printf("cflag = %d, cvalue  = %s,qflag = %d, qvalue  = %s,  gflag = %d, gvalue  = %s ,dflag = %d, dvalue  = %s ,\n uflag = %d, uvalue  = %s, mflag = %d, mvalue  = %s, rflag = %d, rvalue  = %s, tot_flags %d \n",
	 cflag, cvalue, qflag, qvalue, gflag, gvalue, dflag, dvalue, uflag,
	 uvalue, mflag, mvalue, rflag, rvalue, tot_flags);

  /*for (index = optind; index < argc; index++) 
     printf ("Agenda : %s\n", argv[index]); */

  printf ("Agenda : %s\n", argv[optind]);
  #endif
  
  for (index = optind + 1; index < argc; index++)
    {
	  printf("Errore : troppi file agenda già specificato. Questa è una non-opzione di troppo : %s\n",argv[index]);
	  agendemul++;
    }
	
  if (agendemul)
    return -1;
	
	
  #ifdef DEBUG
  fprintf(stderr, "CHECKPOINT 2 \n");
  #endif	
	
	
  if(tot_flags == 1)
    {
		if(cflag == 1)
			{
			*msg=MSG_MKAGENDA;
			*buffer=cvalue;
			*lunghezza=strlen(cvalue);
			return MSG_MKAGENDA;
			}
		if(qflag == 1)
			{
			*msg=MSG_RMAGENDA;
			*buffer=qvalue;
			*lunghezza=strlen(qvalue);
			return MSG_RMAGENDA;	
			}
		if(rflag == 1)
			{
			*msg=MSG_RMPATTERN;
			sprintf(*buffer,"%s%c%s",argv[optind],'\0',rvalue);
			*lunghezza=strlen(argv[optind])+strlen(rvalue);
			return MSG_RMPATTERN;	
			}
		if(gflag == 1)
			{
			*msg=MSG_EGIORNO;
			sprintf(*buffer,"%s%c%s",argv[optind],'\0',gvalue);
			*lunghezza=strlen(argv[optind])+strlen(gvalue);
			
			return MSG_EGIORNO;	
			}
		if(mflag == 1)
			{
			*msg=MSG_EMESE;
			sprintf(*buffer,"%s%c%s",argv[optind],'\0',mvalue);
			
			*lunghezza=strlen(argv[optind])+strlen(mvalue);
			
			return MSG_EMESE;	
			}
	}	
		
		
	if(tot_flags == 2)
		{
		*msg=MSG_INSERT;
		sprintf(*buffer,"%s%c%s%c%s%c%s",argv[optind],'\0',dvalue,'\0',utente,'\0',descrizione);
		fprintf(stderr,"il buffer sarà %s %s %s %s\n",argv[optind],dvalue,utente,descrizione);
				
		*lunghezza=strlen(argv[optind])+strlen(dvalue)+strlen(utente)+strlen(descrizione);
		
		return MSG_INSERT;	
		}
	

  return 0;

}

int to_normal_string(char* ingresso, int lunghezza,char **new){
		int app=lunghezza;
		
		int tokens=1; /*  a questa funzione faccio anche calcolare quanti tokens ci sono*/			
		
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
		
		#ifdef DEBUG
		fprintf(stderr,"finito");
		#endif
		return tokens;
		
		}


