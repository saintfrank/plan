/**
 * \file
 * \author lcs08
 * \brief agenda come lista di eventi
 *
 * Rappresentazione di una agenda come lista concatenata di eventi e funzioni manipolazione lista
 */

#ifndef __LIST_H__
#define __LIST_H__

/** Nodo della lista che raccoglie gli eventi di una agenda */
typedef struct elem_t {
/** puntatore all'evento */
    evento_t* ptev;
/** puntatore all'elemento successivo */
    struct elem_t* next;
}
elem_t;



/**
 Aggiunge un nuovo evento alla lista di eventi

 \param agenda il puntatore al puntatore alla testa della lista
               in cui inserire l'evento

 \param ev puntatore all'evento da inserire

 \retval 0 se l'inserzione e' andata a buon fine
 \retval 1 altrimenti
 */
int add(elem_t** agenda, evento_t* ev);

/** Restituisce la lista degli eventi che contiene tutti gli eventi che hanno un match con una data
 \param data la data di cui dobbiamo cercare il match (es 12-12-1997 oppure **-12-1997)
 \param agenda l'agenda in cui cercare
 \param trovati il puntatore al puntatore alla testa della lista risultato della ricerca

 \retval n il numero di elementi contenuti nella lista risultato oppure
 \retval -1 in caso di errore
 */
int cerca(char data[], elem_t* agenda, elem_t** trovati);

/** Rimuove dalla lista gli eventi che hanno un match con il pattern richiesto in uno qualsiasi dei campi
 \param pattern il pattern da cercare
 \param agenda l'agenda in cui cercare

 \retval p il puntatore alla lista aggiornata
 */
elem_t* rimuovi(char pattern[], elem_t* agenda);

/**
 * Legge il file ingresso e inserisce tutti i record  in una lista concatenata

 \param ingresso il file di ingresso
 \param agenda il puntatore al puntatore alla testa della lista risultato del caricamento

 \retval n il numero dei record letti ed inseriti nella lista se tutto e' andato a buon fine
 \retval -1 altrimenti
 */
int loadAgenda(FILE* ingresso, elem_t** agenda);

/**
 Scrive tutti i record della lista 'agenda' nel file 'uscita' secondo il formato predefinito

 \param uscita il file su cui scrivere il contenuto della lista
 \param agenda il puntatore alla testa della lista

 \retval n il numero di voci registrate oppure
 \retval -1 se si e' verificato un errore
 */
int storeAgenda(FILE* uscita, elem_t* agenda);

/**
  dealloca tutta la memoria occupata dalla lista

  \param lista puntatore al primo elemento della lista da deallocare
 */
void dealloca_lista(elem_t* lista);
#endif
