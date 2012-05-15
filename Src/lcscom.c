#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include "lcscom.h"

#define DEBU

#define UNIX_PATH_MAX   108
#define MAXMESSAGEBUFFER 125


/** Crea un channel di ascolto AF_UNIX
 *  \param  path pathname del canale di ascolto da creare
 *  \return
 *     - s    il descrittore del channel di ascolto  (s>0)
 *     - -1   in casi di errore (sets errno)
 *     - SOCKNAMETOOLONG se il nome del socket eccede UNIX_PATH_MAX
 */
serverChannel_t createServerChannel(const char* path){
	
	serverChannel_t new_fd_skt;
	struct sockaddr_un indirizzo ;
	int errbind, errlist=0;
	errbind = 0; 
	
	if(path == NULL)
		return -1;
	
	/* controllo la lunghezza del path */
	if(strlen(path)>UNIX_PATH_MAX)
		return SOCKNAMETOOLONG;
	
	
	strcpy(indirizzo.sun_path, path);
	indirizzo.sun_family = AF_UNIX ;
	
	new_fd_skt=socket(AF_UNIX,SOCK_STREAM,0); 	/* apro il server socket */

	errbind=bind(new_fd_skt, (struct sockaddr *) &indirizzo, sizeof(indirizzo)) ;		/* connetto il server col nome */

	perror("createServerChannel: bind problem");

	errlist = listen( new_fd_skt , SOMAXCONN ) ;
			
	perror("createServerChannel: listen problem (questo errore è ignorabile, la perror non lavora bene con la listen)");
	
	
	
	if(errbind !=0 || errlist != 0)
		return -1;
		else{
			#ifdef DEBUG
			fprintf(stderr,"create - bind  %d, listen %d, socket %d!\n",errbind, errlist, (int) new_fd_skt);
			#endif
			return new_fd_skt;
			}
	
	}

/** Distrugge un channel di ascolto
 *   \param s il channel di ascolto da distruggere
 *   \return
 *         -  0  se tutto ok, 
 *         - -1  se errore (sets errno)
 */
int closeSocket(serverChannel_t s){
	
	if(close(s)==-1)
		return -1;
		else{
			#ifdef DEBUG
			fprintf(stderr, "chiudo %d  !\n",(int) s);
			#endif
			return 0;
			}	
	}

/** accetta una connessione da parte di un client
 *   \param  s channel di ascolto su cui si vuole ricevere la connessione
 *   \return
 *       - c il descrittore del channel di trasmissione con il client 
 *       - -1 in casi di errore (sets errno)
 */
channel_t acceptConnection(serverChannel_t s){
	
	struct sockaddr_un addr;

	socklen_t x2 = sizeof (addr);  /* per la accept */
	
	channel_t c;
	
	if((c = accept(s, (struct sockaddr *)&addr, &x2)) == -1)         /* aspetta finchè non c'è una connessione */
		perror("errore dispatcher 's accept :");

	

	#ifdef DEBUG
	fprintf(stderr,"LCSCOM - accept andata, file descriptor: %d  !\n",(int)c);
	#endif

	return c;
	
	}

/** legge un messaggio dal channel di trasmissione
 *  \param  sc  channel di trasmissione
 *  \param msg  struttura che conterra' il messagio letto 
 *		(deve essere allocata all'esterno della funzione,
 *		tranne il campo buffer)
 *  \return
 *     -  -1    se errore (errno settato),
 *     -  SEOF  se EOF sul socket 
 *     -  lung  lunghezza del buffer letto, se OK
 */
int receiveMessage(channel_t sc, message_t *msg){
	
	int nread;
	char *nuova_posizione= calloc(MAXMESSAGEBUFFER,sizeof(char));
	
	struct {
		message_t messaggio; 
		char stringa[MAXMESSAGEBUFFER] ;
		} buf;
	
		
	nread = read(sc, &buf, sizeof(message_t)+(sizeof(char)*MAXMESSAGEBUFFER)) ; /* è arrivato un messaggio da un client*/
		perror("read");
    
	memcpy(nuova_posizione,&(buf.stringa),MAXMESSAGEBUFFER); /**/
	buf.messaggio.buffer=nuova_posizione;
	
	#ifdef DEBUG
	fprintf(stderr,"LCSCOM - read superata %d %d %c %d %s!\n",nread,sc,buf.messaggio.type,buf.messaggio.length,buf.stringa);
	#endif
	
	
	memcpy(msg,&(buf.messaggio),sizeof(message_t));
	
	
	if(nread<10)
		return SEOF;
		else 
		return nread; 	
	
	}
/** scrive un messaggio sul channel di trasmissione
 *   \param  sc channel di trasmissione
 *   \param msg struttura che contiene il messaggio da scrivere 
 *   \return
 *     -  -1   se errore (sets errno) 
 *     -  n    il numero di caratteri inviati (se OK)
 */
int sendMessage(channel_t sc, message_t *msg){
	
	int nwrite;	

	struct {
		message_t messaggio; 
		char stringa[MAXMESSAGEBUFFER];
		} buf;	
		
	if(msg == NULL)
		return -1;
				
	#ifdef DEBUG
	fprintf(stderr,"LCSCOM - check 1 !\n");
	#endif	
	
	memcpy(&(buf.messaggio),msg,sizeof(message_t));
	
	#ifdef DEBUG
	fprintf(stderr,"LCSCOM - send memcpy andata %c %d !\n",buf.messaggio.type, buf.messaggio.length);
	#endif
	
	
	memcpy(&(buf.stringa), msg->buffer,MAXMESSAGEBUFFER);	
	
	#ifdef DEBUG
	fprintf(stderr,"LCSCOM - strcpy andata %s !\n",buf.stringa);
	#endif		
	
	nwrite = write(sc, &buf, sizeof(message_t)+(sizeof(char)*MAXMESSAGEBUFFER)) ;								  /* scrive il buffer nel socket */
	
	#ifdef DEBUG
	fprintf(stderr,"ho stampato %d caratteri msg, %s\n",nwrite,buf.stringa);
	#endif
	
	return nwrite;
	
	}
/** Chiude un socket
 *   \param  sc il descrittore del channel da chiudere 
 *   \return
 *       - 0  se tutto ok
 *       - -1 se errore (sets errno)
*/
int closeConnection(channel_t sc){
	
	if(close(sc)==EOF)
		return -1;
		else{
			#ifdef DEBUG
			fprintf(stderr, "chiudo %d  !\n",(int) sc);
			#endif
			return 0;
			}		
	
	}
/** crea un channel di trasmissione verso il server
 *   \param  path  nome del server socket
 *   \return
 *      -  c (c>0) il channel di trasmissione 
 *      - -1 in casi di errore (sets errno)
 *      - SOCKNAMETOOLONG se il nome del socket eccede UNIX_PATH_MAX
 */
channel_t openConnection(const char* path){
	
	struct sockaddr_un indirizzo;
	channel_t to_server;

	int i;
	strcpy(indirizzo.sun_path, path);
	indirizzo.sun_family = AF_UNIX ;
	
	to_server = socket(AF_UNIX, SOCK_STREAM, 0) ;			/* apre un socket verso il server */
	
	for (i =0;i<5;i++)         
		if (connect(to_server, (struct sockaddr *)&indirizzo, sizeof(indirizzo)) == -1)  /* connette il suo socket ( provando a distanza di un secondo ) usando l'indirizzo dato */
	         {
			    if (errno == ENOENT) {
	                sleep(1);
	                return -1;
	             }else{
						sleep(1); 
						perror("nessuna risposta, riprovo a contattare il server \n");
					  }
			 }else 
				return to_server;
				
	return -1;
	
	}
