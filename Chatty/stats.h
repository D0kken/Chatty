/**
 *   Stefano Russo 544341
 *
 *   stefanoruss97@gmail.com   
 *
 *  Si dichiara che il contenuto di questo file e' in ogni sua parte opera
 *    originale dell'autore
 */



/*
 * chatterbox Progetto del corso di LSO 2017/2018
 *
 * Dipartimento di Informatica Università di Pisa
 * Docenti: Prencipe, Torquati
 * 
 */
#ifndef MEMBOX_STATS_
#define MEMBOX_STATS_

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <config.h>
#include <extern.h>



extern  struct statistics  chattyStats;

extern message_t *MSGS;
extern const int   msgbatch;
extern  int      msgcur;
extern  int      msglen;     
extern  int      msgmax;

/* aggiungere qui altre funzioni di utilita' per le statistiche */


/**
 * @function printStats
 * @brief Stampa le statistiche nel file passato come argomento
 *
 * @param fout descrittore del file aperto in append.
 *
 * @return 0 in caso di successo, -1 in caso di fallimento 
 */
 int printStats(FILE *fout) {
   


   
    if (fprintf(fout, "%ld - %ld %ld %ld %ld %ld %ld %ld\n",
    (unsigned long)time(NULL),
    chattyStats.nusers, 
    chattyStats.nonline,
    chattyStats.ndelivered,
    chattyStats.nnotdelivered,
    chattyStats.nfiledelivered,
    chattyStats.nfilenotdelivered,
    chattyStats.nerrors
    ) < 0) return -1;
    fflush(fout);
    return 0;
}




/**
 * @function statIncr
 * @brief Incrementa o decrementa una determinata statistica 
 *
 * @param stats statistica da incrementare
 * @param inc   valore di incremento
 *
 * @return  0 in caso di successo
            -1 in caso di fallimento 
 */


int statIncr(unsigned long *stats,int inc );



int statIncr(unsigned long *stats,int inc ){

 if(pthread_mutex_lock(&chattyStats.statlock)!=0)
  {
    printf("statlock mutex errore");
      return -1;
  }


if(inc==0)*stats=0;

else *stats=*stats + inc;



 if(pthread_mutex_unlock(&chattyStats.statlock)!=0)
  {
    printf("statlock mutex errore");
      return -1;
  }

if(*stats<0)return -1;

return 0;
}





#endif /* MEMBOX_STATS_ */

