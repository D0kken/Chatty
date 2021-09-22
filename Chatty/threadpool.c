/**
 *   Stefano Russo 544341
 *
 *   stefanoruss97@gmail.com   
 *
 *  Si dichiara che il contenuto di questo file e' in ogni sua parte opera
 *    originale dell'autore
 */


#include <errno.h>
#include <signal.h>
#include <stdlib.h>

#include <config.h>
#include <extern.h>






threadpool_t *threadpool_create(int thread_count, int queue_size)
{
    threadpool_t *pool;
    
    int i=0;
    

    if(thread_count <= 0 || thread_count > ThreadsInPool || queue_size <= 0 || queue_size > MaxConnections)
     { ///numero th,e grandezza coda fuori dai limiti
        return NULL;
    }

    if((pool = (threadpool_t *)malloc(sizeof(threadpool_t))) == NULL) 
    {
         if(pool) 

             threadpool_free(pool);
    
         return NULL;
    }

    
    pool->thread_count = 0;

    pool->queue_size = queue_size;

    pool->head = pool->tail = pool->count = 0;
    

    
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
   
    pool->queue = (threadpool_task *)malloc
        (sizeof(threadpool_task) * queue_size);

    
    if((pthread_mutex_init(&(pool->lock), NULL) != 0) ||
       (pthread_cond_init(&(pool->notify), NULL) != 0) ||
       (pool->threads == NULL) ||
       (pool->queue == NULL)) 
    {
        
     if(pool) 
        threadpool_free(pool);
    
    return NULL;
    }

    
    for(i = 0; i < thread_count; i++)
     {
        if(pthread_create(&(pool->threads[i]), NULL,threadpool_thread, (void*)pool) != 0)
         {   //threaadpool_thread funzione di partenza del th
            threadpool_destroy(pool);

            return NULL;
        }
        pool->thread_count++;
       
    }

    return pool;

}










int threadpool_add(void (*function),void *argument,int localcurr)
{

int err = 0;   //valore di ritorno della funzione
int next=0;    //task successivo


   if( function == NULL)
    {
        
        printf("[-]function not valid \n");
        return -1;
    }

   


    if(pool == NULL)
     {
        
        printf("[-]pool  not valid \n");
        return -1;
    }







    if(pthread_mutex_lock(&(pool->lock)) != 0) 
    {
    
    	printf("[-]mallock not valid \n");
        return -1;
    }


//incremento di uno il task sucessivo,se arriva al massimo torna a 0
    next = (pool->tail + 1) % pool->queue_size;


//se il numero di task è = al numero massimo di task,l'aggiunta del task viene rifiutata
    do {
        
        if(pool->count == pool->queue_size) 
        {
            err = -1;
            break;
        }

       
//aggiungo il task in coda
        pool->queue[pool->tail].function = function;

        pool->queue[pool->tail].argument = argument;

	    pool->queue[pool->tail].localcur = localcurr;//posizione nel MSGS di questa richiesta 
 
        pool->tail = next;

        pool->count += 1;



        
        if(pthread_cond_signal(&(pool->notify)) != 0)
         {
            err = -1;
            break;
        }
    } while(0);

    if(pthread_mutex_unlock(&pool->lock) != 0)
    {
        err = -1;
    }

    return err;
}






int threadpool_destroy(threadpool_t *pool)
{
	
int i=0, err = 0;


    if(pool == NULL)
     {
        return -1;
    }


    if(pthread_mutex_lock(&(pool->lock)) != 0) 
    {
        return -1;
    }

  

    do {
     //setto un numero fasullo del count cosi' quando si sveglia il th non si riaddormenta,infatti deve suicidarsi
	 pool->count=1;


      
        //sveglio tutti i thread,cosi' terminano 
        if((pthread_cond_broadcast(&(pool->notify)) != 0) || 
           (pthread_mutex_unlock(&(pool->lock)) != 0)) 
        {
            err = -1;;
            break;
        }



        //aspetto che tutti i thread terminino
        for(i = 0; i < pool->thread_count; i++) 
        {
        	printf("[Threadpool_destroy]Aspetto th: %08x\n",(unsigned int)pool->threads[i]);
            if(pthread_join(pool->threads[i], NULL) != 0) 
                err = -1;//errno=ENOENT;
            
        }
    } while(0);



   //se non ci sono problemi,libero la memoria del pool
    if(!err) 
        threadpool_free(pool);
    
    return err;
}







int threadpool_free(threadpool_t *pool)
{


    if(pool == NULL ) 
        return -1;
    


    if(pool->threads) 
    {
        free(pool->threads);
        free(pool->queue);
 
      
        pthread_mutex_destroy(&(pool->lock));
        pthread_cond_destroy(&(pool->notify));
    }
    free(pool);    

    return 0;
}





 void *threadpool_thread(void * threadpool)
{



    threadpool_t *pool = (threadpool_t *)threadpool;

    threadpool_task task;

    while(TRUE) {

	
    	
        pthread_mutex_lock(&(pool->lock));

        //se non ci sono elementi in coda mi metto in attesa su notify e rilascio la mutex
        while(pool->count == 0) {
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }


        //al risveglio se un segnale è stato localizzato,esco dal loop lasciando la mutex e terminando 
        if(terminazione !=0 && terminazione != SIGUSR1)
        {
        	break;
        }		

  	    //se quando mi sveglio c'è da fare,la faccio
        if(pool->count != 0) 
        {
            
        
        
        task.function = pool->queue[pool->head].function;
        
        task.argument = pool->queue[pool->head].argument;
        
        task.localcur= pool->queue[pool->head].localcur;
        
        pool->head = (pool->head + 1) % pool->queue_size;
        
        pool->count -= 1;


       
        pthread_mutex_unlock(&(pool->lock));

        
        task.function(task.argument,task.localcur);

       }

      else 
        pthread_mutex_unlock(&(pool->lock));
    
    }

    //se sono uscito vuol dire che c'è un segnale,rilascio dunque la mutex e termino il thread
  
    pthread_mutex_unlock(&(pool->lock));
  

    return(NULL);

}

