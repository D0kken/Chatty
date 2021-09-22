/**
 *   Stefano Russo 544341
 *
 *	 stefanoruss97@gmail.com   
 *
 *  Si dichiara che il contenuto di questo file e' in ogni sua parte opera
 *    originale dell'autore
 */


#ifndef EXTERN_H_
#define EXTERN_H_

#include <stdio.h>
#include <pthread.h>
#include <connessi.h>
#include <hash_tables.h>
#include <threadpool.h>
#include <message.h>



struct statistics {
    unsigned long nusers;                       // n. di utenti registrati
    unsigned long nonline;                      // n. di utenti connessi
    unsigned long ndelivered;                   // n. di messaggi testuali consegnati
    unsigned long nnotdelivered;                // n. di messaggi testuali non ancora consegnati
    unsigned long nfiledelivered;               // n. di file consegnati
    unsigned long nfilenotdelivered;            // n. di file non ancora consegnati
    unsigned long nerrors;                      // n. di messaggi di errore
    pthread_mutex_t statlock;                   //mutex per poter aggiornare le statistiche 
};



extern message_t *MSGS;
extern const int   msgbatch;
extern  int      msgcur;
extern  int      msglen;     
extern  int      msgmax;
 

extern pthread_mutex_t connlock,htrlock,htmlock,backlock;

//extern int segnale_exit;  //per il controllo dei segnali ricevuti
//extern int sigcount;	  //contatore di segnali ricevuti
extern int terminazione; //variabile di terminazione

extern hash* ht_registered;   // hash table contentente gli utenti registrati

extern hash* ht_msgs;		  //hash table contentente i messaggi degli utenti


extern threadpool_t* pool;   //pool di thread che eseguono le richieste dei client


extern struct sockaddr_un sa; //sa per il socket di connessione
extern int master_socket;    //socket principale 
 
extern struct statistics chattyStats;


extern Connessi* conn;         //struttura connessi  

extern char* UnixPath;
extern int   MaxConnections;
extern int   ThreadsInPool;  
extern int   MaxMsgSize;   
extern int   MaxFileSize;      
extern int   MaxHistMsgs;      
extern char* DirName;
extern char* StatFileName;     
extern struct sigaction saN;




#endif
