/**
 *   Stefano Russo 544341
 *
 *	 stefanoruss97@gmail.com   
 *
 *  Si dichiara che il contenuto di questo file e' in ogni sua parte opera
 *    originale dell'autore
 */


#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>

#include <stats.h> 
#include <connections.h>
#include <listenerworker.h>
#include <connessi.h>
#include <parsing_config.h>




static void usage(const char *progname) {
    fprintf(stderr, "Il server va lanciato con il seguente comando:\n");
    fprintf(stderr, "  %s -f conffile\n", progname);
}


int terminazione=0;
 

volatile sig_atomic_t  segnale_exit=0; //tiene conto del valore del segnale ricevuto
volatile sig_atomic_t  sigcount=0;	 //tiene conto del numero di segnali ricevuti


//nuovo handler per alcuni segnali
static void sig_handler(int sig){

	switch(sig){

		case SIGINT:
				sigcount++;
				
				segnale_exit=SIGINT;
		break;

		case SIGTERM:
				sigcount++;
			
				segnale_exit=SIGTERM;
		break;

		case SIGQUIT:
				sigcount++;
		
				segnale_exit=SIGQUIT;
		break;

		case SIGUSR1:
				sigcount++;
			
				segnale_exit=SIGUSR1;
		break;
	}



}



struct statistics  chattyStats = { 0,0,0,0,0,0,0 };


  message_t *MSGS=NULL;
  const int   msgbatch=100;
  int      msgcur=0;
  int      msglen=0;     
  int      msgmax=100;
 



  hash* ht_registered;   // hash table contentente gli utenti registrati

  hash* ht_msgs;		   //hash table contentente i messaggi pendenti nei confronti dei client




 pthread_mutex_t connlock,htrlock,htmlock;  //mutex per accedere alla lista dei connessi,registrati e messaggi



  threadpool_t* pool;   //pool di thread 

 char* UnixPath="init";
 int   MaxConnections=0;
 int   ThreadsInPool=0;  
 int   MaxMsgSize=0;   
 int   MaxFileSize=0;      
 int   MaxHistMsgs=0;      
 char* DirName;
 char* StatFileName;     

struct sockaddr_un sa; //sa per il socket di connessione
int master_socket=0;    //socket principale
 


Connessi* conn;         //struttura connessi contenente array di utenti,array di file descriptor , numero connessi e ind per diminuire la ricerca 






void cleanup() {
    unlink(UnixPath);
    libera_spazzatura();

}







void* ending(){

int fd=0;

terminazione=segnale_exit;
fd=openConnection(UnixPath, 5,1);

close(fd);

return NULL;


}





     
int main(int argc , char *argv[])   
{   

 cleanup();  

 atexit(cleanup); 

puts("Salve");


  if(argc != 3 || strcmp(argv[1], "-f") != 0) 
   {
    usage(argv[0]);

    return -1;
   }
 int w=0;

 
printf("Caricamento configurazione..."); 
parsing_config( argv[2] );
puts("completato");

conn=connessi_create();





 int opt = TRUE;

printf("Inizializzazione variabili...");
 MSGS=calloc(1,(msgcur+ msgbatch)*sizeof(message_t)); //creo 100 posti messaggi


 pthread_t tid;//listener
 pthread_t tid2;//ender
 



  

if(pthread_mutex_init(&backlock,NULL)!=0 ||pthread_mutex_init(&connlock,NULL)!=0 ||pthread_mutex_init(&htrlock,NULL)!=0 ||pthread_mutex_init(&htmlock,NULL)!=0 )
	{
		printf("errore inizializzazione mutex");
  		return 0;
    }



//inizializzo MSGS

for( w=0;w<msgmax;++w)
  {
 MSGS[w].data.buf=NULL;	
 MSGS[w].hdr.op  =FREE_BLOCK;	

  }


for(w=0;w<MaxConnections;w++){
	conn->file_des[w]=0;

  }

 
//inizializzo hast table di utenti registrati
 ht_registered=hash_create(); 


//inizializzo hast table dei messaggi
 ht_msgs=hash_create();


//utilizzo un nuovo handler per alcuni segnali,altri vengono ignorati
 struct sigaction saN;

 memset(&saN,0,sizeof(saN));

saN.sa_handler=sig_handler;

sigaction(SIGINT,&saN,NULL);
sigaction(SIGQUIT,&saN,NULL);
sigaction(SIGTERM,&saN,NULL);
sigaction(SIGUSR1,&saN,NULL);

 saN.sa_handler=SIG_IGN;
sigaction(ECONNREFUSED,&saN,NULL);
sigaction(ECONNRESET,&saN,NULL);
sigaction(SIGPIPE,&saN,NULL);
sigaction(EPIPE,&saN,NULL);



if( (master_socket= socket(AF_UNIX , SOCK_STREAM , 0)) == 0)   
  {   
    perror("socket failed");   
        
    exit(EXIT_FAILURE);   
  }   
     
 
	strncpy(sa.sun_path,UnixPath,UNIX_PATH_MAX);
	sa.sun_family=AF_UNIX;
  
         

if (bind(master_socket, (struct sockaddr *)&sa, sizeof(sa))<0)   
{   
    perror("bind failed");   
       
     exit(EXIT_FAILURE);   
} 

 


if (listen(master_socket,MAXBACKLOG ) < 0)   
 {   
    perror("listen");   
       
     exit(EXIT_FAILURE);   
 }   
         
 

 pool= threadpool_create(ThreadsInPool,MaxConnections);

if(pool==NULL) 
  return -1;

//creo th listener che con una select gestisce le connessioni da parte dei client
    
 if ( pthread_create(&tid,NULL,th_Listener,NULL) != 0) 
  {
    perror("pthread_create: \n");
    exit(EXIT_FAILURE);
  }


puts("completato.");//inizializzazione


while( !segnale_exit ||  segnale_exit==SIGUSR1){


if(segnale_exit==SIGUSR1)
  {
      //scrivo nel file le statistiche del server
 
         chattyStats.nusers  =ht_registered->nkeys;   // n. di utenti Registrati  
         chattyStats.nonline =conn->nconn;         // n. di utenti connessi                

         
             segnale_exit=0;

             FILE* fout;
             fout=fopen(StatFileName,"a");
              
               if( fout==NULL ) 
               
              perror("Errore in apertura del file");
            

             printStats(fout) ;


             fclose(fout);


             //resetto il segnale 
        
 



  }

else sleep(1);
     
 
}








//creazione thread ender
if ( pthread_create(&tid2,NULL,ending,NULL) != 0) 
  {
    perror("pthread_create: \n");
    exit(EXIT_FAILURE);
  }


puts("[+]ender created");


  if (pthread_join(tid2, NULL) != 0) 
  {
    perror("pthread_join : \n");
    exit(EXIT_FAILURE);
  }



  if (pthread_join(tid, NULL) != 0) 
  {
    perror("pthread_join : \n");
    exit(EXIT_FAILURE);
  }




printf("Thread Listener exited \n");//ret contiene lo status con cui esce il thread con la  pthread_exit(ret)

printf("Pulizia.....\n");



puts("[+]Eseguo la threadpool_destroy");

 if(threadpool_destroy(pool)!=0)   				//distrugge il threadpool aspettando che tutti i worker finiscano di lavorare
 	perror("[-]threadpool_destroy: ");


puts("[+]Threadpool_destroy: completata");




puts("[+]Eseguo cancellazione MSGS");


free(MSGS);

puts("[+]Cancellazione MSGS: completata");




puts("[+]Eseguo cancellazione Connessi");




for(w=0;w<MaxConnections;w++)
{
  
if(conn->Conness[w]!=NULL)
 {
  
  free(conn->Conness[w]);
 }
}
free(conn);


puts("[+]Cancellazione Connessi: completata");







puts("[+]Eseguo hash_destroy");


lista *curr;
 if(ht_registered == NULL) 
  {
    errno = EINVAL;
    return -1;
  }

  for(size_t i = 0; i < MAX_CELLE; i++) 

  {

    pthread_mutex_destroy(&ht_registered->trab[i]);

    if(ht_registered->cella[i] != NULL) 
    {
      for(curr=ht_registered->cella[i];curr!=NULL;curr=curr->next)
      {
         free(ht_registered->cella[i]->key);
  
      }
    free(ht_registered->cella[i]); 
    }
  
  }
 


 free(ht_registered);

 
 hash_destroy(ht_msgs);



 puts("[+]Hash_destroy: completata");

 pthread_mutex_destroy(&backlock);
 pthread_mutex_destroy(&htrlock);
 pthread_mutex_destroy(&htmlock);
 pthread_mutex_destroy(&connlock);
 pthread_mutex_destroy(&chattyStats.statlock);



 puts("Arrivederci.");


return 0;

}




 