/**
 *   Stefano Russo 544341
 *
 *	 stefanoruss97@gmail.com   
 *
 *  Si dichiara che il contenuto di questo file e' in ogni sua parte opera
 *    originale dell'autore
 */


#ifndef LEW_H_
#define LEW_H_


#include<config.h>
#include<extern.h>




/**
 * @function Gestione
 * @brief Funzione di gestione eseguita da un thread che gestiste la richiesta di un client
 *
 * @param argo       file descriptor associato al nickname che porge la richiesta
 * @param localcurr indice di MSGS contenente la richiesta 
 *
*/
void Gestione(void* fd, int localcurr);


/**
 * @function th_Listener
 * @brief Funzione eseguita dal thread Listener,per l'ascolto di nuove richieste da parte dei client
 */
void *th_Listener() ;




#endif //LEW