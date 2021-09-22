/**
 *   Stefano Russo 544341
 *
 *	 stefanoruss97@gmail.com   
 *
 *  Si dichiara che il contenuto di questo file e' in ogni sua parte opera
 *    originale dell'autore
 */


#ifndef CONN_H_
#define CONN_H_





//struttura connessi
 typedef struct {

 char* Conness[32];   //array contenente i nomi degli utenti connessi
 int   file_des[32];  //array contenente i file descriptor degli utenti connessi
 int   nconn;	      //numero connessi
 int   ind;			  //indice per l'aggiunta di un nuovo elemento nella struttura

}Connessi ;


 
/**
 * @function IsConnesso
 * @brief Controlla se un nickname è connesso
 * @param nick       nickname da controllare
 * 
 *@returno  0 se l'utente è connessi
 *			-1 altrimenti
*/

int IsConnesso(char*nick);



/**
 * @function return_connessi
 * @brief Procura la lista di utenti connessi
 *
 * @param localcurr indice di MSGS contenente la richiesta 
 *
*/

char* return_connessi(int localcurr);



/**
 * @function connessi_create
 * @brief Crea una struttura Connessi
 *
 * @param localcurr indice di MSGS contenente la richiesta 
 *
*/

Connessi* connessi_create();


#endif // CONN_H_
