/*
 * chatterbox Progetto del corso di LSO 2017/2018
 *
 * Dipartimento di Informatica Università di Pisa
 * Docenti: Prencipe, Torquati
 * 
 */
/**
 *   Stefano Russo 544341
 *
 *   stefanoruss97@gmail.com   
 *
 *  Si dichiara che il contenuto di questo file e' in ogni sua parte opera
 *    originale dell'autore
 */
#ifndef CONNECTIONS_H_
#define CONNECTIONS_H_


#if !defined(UNIX_PATH_MAX)
#define UNIX_PATH_MAX  64
#endif

#include<stdio.h>
#include<message.h>





/**
 * @file  connection.h
 * @brief Contiene le funzioni che implementano il protocollo 
 *        tra i clients ed il server
 */




/**
 * @function openConnection
 * @brief Apre una connessione AF_UNIX verso il server 
 *
 * @param path Path del socket AF_UNIX 
 * @param ntimes numero massimo di tentativi di retry
 * @param secs tempo di attesa tra due retry consecutive
 *
 * @return il descrittore associato alla connessione in caso di successo
 *         -1 in caso di errore
 */

int openConnection(char* path, unsigned int ntimes, unsigned int secs);




// -------- server side ----- 
/**
 * @function readHeader
 * @brief Legge l'header del messaggio
 *
 * @param connfd     descrittore della connessione
 * @param hdr    puntatore all'header del messaggio da ricevere
 *
 * @return <=0 se c'e' stato un errore 
 *         (se <0 errno deve essere settato, se == 0 connessione chiusa) 
 */

int readHeader(int connfd, message_hdr_t *hdr);







/**
 * @function readData
 * @brief Legge il body del messaggio
 *
 * @param fd     descrittore della connessione
 * @param data   puntatore al body del messaggio
 *
 * @return <=0 se c'e' stato un errore
 *         (se <0 errno deve essere settato, se == 0 connessione chiusa) 
 */

int readData(int fd, message_data_t *data);







/**
 * @function readMsg
 * @brief Legge l'intero messaggio
 *
 * @param fd     descrittore della connessione
 * @param data   puntatore al messaggio
 *
 * @return <=0 se c'e' stato un errore
 *         (se <0 errno deve essere settato, se == 0 connessione chiusa) 
 */
int readMsg(int fd, message_t *msg);






// ------- client side ------
/**
 * @function sendRequest
 * @brief Invia un messaggio di richiesta al server 
 *
 * @param fd     descrittore della connessione
 * @param msg    puntatore al messaggio da inviare
 *
 * @return <=0 se c'e' stato un errore
 */
int sendRequest(long fd, message_t *msg);



/**
 * @function sendData
 * @brief Invia il body del messaggio al server
 *
 * @param fd     descrittore della connessione
 * @param msg    puntatore al messaggio da inviare
 *
 * @return <=0 se c'e' stato un errore
 */
int sendHeader(long fd, message_hdr_t *msg);




/**
 * @function sendData
 * @brief Invia il body del messaggio al server
 *
 * @param fd     descrittore della connessione
 * @param msg    puntatore al messaggio da inviare
 *
 * @return <=0 se c'e' stato un errore
 */


int sendData(long fd, message_data_t *msg);


/* da completare da parte dello studente con eventuali altri metodi di interfaccia */



/**
 * @function HeaderMsgs
 * @brief legge l'header del messaggio 
 *
 * @param fd     descrittore della connessione
 * @param hdr    puntatore all'header in cui salvare il messaggio
 *
 * @return <=0 se c'e' stato un errore
 */

int HeaderMsgs(char* fd, message_hdr_t *hdr);





/**
 * @function DataMsgs
 * @brief legge il body del messaggio
 *
 * @param data   puntatore al data in cui salvare il messaggio
 * @param fd     descrittore della connessione
 *
 * @return <=0 se c'e' stato un errore
 */

int DataMsgs(message_data_t *data,int fd);




/**
 * @function DataFile
 * @brief Riceve il contenuto del file inviato dal client
 *
 * @param fd     descrittore della connessione
 * @param data   puntatore al data in cui salvare il messaggio
 * @param		 nome del file da ricevere	
 * @MaxFileSize  valore della grandezza massima di un file
 *
 * @return <=0 se c'e' stato un errore
 */
 int DataFile(int fd, message_data_t *data,char*nome,int MaxFileSize);




/**
 * @function fbackup
 * @brief salva il puntatore di una stringa passata come argomento
 *
 * @param string stringa di cui salvare il puntatore
 * @return 0 se c'e' stato un errore
 *		   1 in caso di successo
 */

 int fbackup( char *string);



/**
 * @function libera_spazzatura
 * @brief libera tutti i puntatori salvati in backup
 *
*/

 void libera_spazzatura();



#endif /* CONNECTIONS_H_ */

