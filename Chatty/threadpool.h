/**
 *   Stefano Russo 544341
 *
 *   stefanoruss97@gmail.com   
 *
 *  Si dichiara che il contenuto di questo file e' in ogni sua parte opera
 *    originale dell'autore
 */



#ifndef POOL_H_
#define POOL_H_



 

typedef struct {

    void *argument;            //Fd della connessione
    void (*function)(void *,int);  //Funzione che gestisce la richiesta
    int localcur;              //per non perdere il messaggio
} threadpool_task;





typedef struct {
  pthread_mutex_t lock;      //Variabile di condizione per notificare i thread worker.
  pthread_cond_t notify;     // Array contenenti  ID dei thread worker.
  pthread_t *threads;        //Numero di  threads
  threadpool_task *queue;  // Coda di richieste.   
  int thread_count;          //Grandezza del pool.
  int queue_size;            //Grandezza della coda.
  int head;                  //Indice della prima richiesta da gestire
  int tail;                  //Indice della prossima richiesta da gestire.
  int count;                 //Numero di richieste pendenti
}threadpool_t ;


/**
 * @function threadpool_create
 * @brief Crea un threadpool_t.
 * @param thread_count Numero di thread worker.
 * @param queue_size   Dimensione della coda.
 * @param flags        Unused parameter.
 * @return thread pool appena creato in caso di successo
           NULL        in caso di fallimento 
 */
threadpool_t *threadpool_create(int thread_count, int queue_size);

/**
 * @function threadpool_add
 * @brief aggiunge una nuova richiesta nella coda del threadpoll.
 * @param function Funzione che gestirà la richiesta.
 * @param argument Argomento passato alla funzione.
 * @return    0 in caso di successo,
             -1 in caso di errore 
  
 */
int threadpool_add(void (*routine),void *arg,int localcurr);

/**
 * @function threadpool_destroy
 * @brief Ferma e distrugge un thread pool.
 * @param pool  Thread pool da distruggere.
 * 
 *@return   -1 in caso di errore
 *           0 in caso di successo
 */
int threadpool_destroy(threadpool_t *pool);




/**
 * @function void *threadpool_thread(void *threadpool)
 * @brief the worker thread
 * @param threadpool the pool which own the thread
 */
 void *threadpool_thread(void *threadpool);


 /**
  * @function threadpool_free
  *@brief  dealloca i thread e la coda
  *@param  pool   pool da deallocare
  *
  *@return -1 in caso di errore
  *         0 in caso di successo
  */
 int threadpool_free(threadpool_t *pool);





#endif//POOL_H_