/**
 * \file
 * \author lcs08
 * \brief rappresentazione eventi
 *
 * Rappresentazione di un evento e funzioni base di conversione/manipolazione
 */
#ifndef __LBASE_H__
#define __LBASE_H__

/** Lunghezza di un record evento su file */
#define LRECORD 100     
/** Lunghezza stringa data */
#define LDATA 10        
/** Lunghezza stringa utente */
#define LUTENTE 8       
/** Lunghezza stringa descrizione */
#define LDESCRIZIONE 80 
/** Lunghezza nome agenda */
#define LAGENDA 20      

/** Rappresentazione di un evento */
typedef struct {
/** data formato gg-mm-aaaa */
  char data[LDATA + 1];
/** utente che ha registrato l'evento */
  char utente[LUTENTE + 1];
/** descrizione evento */
  char descrizione[LDESCRIZIONE + 1];
} 
evento_t;



/**
 Controlla se la data è ha una struttura legale

 \param data	stringa contenente la data

 \retval 0 se la data ha una struttura corretta
 \retval 1 se la data ha una struttura non corretta
 \retval -1 se la data è di dimensioni sbagliate
 */
int isdata(char data[]);


/** Trasforma un record di tipo evento su file 
    es: 01-02-2008 davide\#Bollo auto
    in una struttura di tipo evento    
  \param  r  la stringa che rappresenta l'evento 

  \retval p (puntatore al nuovo evento) se la conversione e' andata a buon fine e non si sono verificati errori
  \retval NULL se si e' verificato un errore (setta errno)
 */
evento_t* convertiRecord(char r[]);



/** Trasforma una struttura di tipo evento in una stringa r nel formato record tipo evento su file  
    es: 01-02-2008 davide\#Bollo auto
  \param r  la stringa da riempire (conterra' il record secondo il formato)
  \param e la struttura evento da convertire

   \retval  0 se tutto e' andato a buon fine 
   \retval -1 altrimenti
 */
int convertiEvento(evento_t* e,char r[]);

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
int matchData(char b[],evento_t* e);


/**
 Controlla se uno dei campi dell'evento contiene un pattern specificato

 \param p  il pattern da cercare (p==NULL da sempre match positivo)
 \param e l'evento da verificare

 \retval 1 se c'e' un matching fra pattern ed evento
 \retval 0 se NON c'e' un matching 
 \retval -1 se si e' verificato un errore 
 */
int matchPattern(char p[],evento_t* e);
#endif
