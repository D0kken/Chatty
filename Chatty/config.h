/*
 * membox Progetto del corso di LSO 2017/2018
 *
 * Dipartimento di Informatica Università di Pisa
 * Docenti: Prencipe, Torquati
 * 
 */

/**
 *   Stefano Russo 544341
 *
 *	 stefanoruss97@gmail.com   
 *
 *  Si dichiara che il contenuto di questo file e' in ogni sua parte opera
 *    originale dell'autore
 */


/**
 * @file config.h
 * @brief File contenente alcune define con valori massimi utilizzabili
 */

#ifndef CONFIG_H_
#define CONFIG_H_



#include<pthread.h>
#include<connessi.h>
#include<hash_tables.h>
#include<threadpool.h>
#include<message.h>





#define MAX_CELLE 5021     //numero massimo di celle nella hash table

#define MAX_RETRIES     10    //numero massimo di tentativi per connessione
#define MAX_SLEEPING     3    //tempo massimo di attesa?
#define TRUE   1              //true
#define FALSE  0              //false
#define MAXBACKLOG  32		   //lunghezza massima coda delle connessioni pendenti (listen)
#define MAX_BACKUP  1024          //lunhgezza struttura di backup




// to avoid warnings like "ISO C forbids an empty translation unit"
typedef int make_iso_compilers_happy;


#endif /* CONFIG_H_ */

