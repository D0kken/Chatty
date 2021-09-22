/**
 *   Stefano Russo 544341
 *
 *   stefanoruss97@gmail.com   
 *
 *  Si dichiara che il contenuto di questo file e' in ogni sua parte opera
 *    originale dell'autore
 */


#ifndef HASH_H_
#define HASH_H_

#define MAX_CELLE 5021 



typedef struct lista_trabocco {

  char *key;                // Chiave (Nickname)
  char *destinatario;
  char *messaggio;              // Messaggio pendente verso il nickname
  int consegnato;
  int fd;                 //fd
  struct lista_trabocco *next;        // Nodo successivo

}lista;

// Tabella hash
 
typedef struct hash_table {
  
  lista* cella[MAX_CELLE]; 				// celle
  int nmes[MAX_CELLE];					//numero messaggi per ogni cella
  pthread_mutex_t trab[MAX_CELLE];
  int nkeys;								//numero di inscritti

 
}hash;

    
    


/**
 * @function fhash
 * @brief Funzione hash per tabella hash registrati 
 *
 * @param string  nickname di cui calcolare la funzione hash 
 *
 * @return valore hash
 */
int fhash(const char *string);



/**
 * @function mfhash
 * @param string  nickname di cui calcolare la funzione hash 
 *
 * @param sender    nickname da aggiungere tra i registrati 
 * @param arg       file descriptor associato al nickname che porge la richiesta
 * @param localcurr indice di MSGS contenente la richiesta 
 *
 * @return valore hash
 */
int mfhash(const char *string);





/**
 * @function mfhash
 * @param string  incrementa o decrementa il contatore dei registrati
 *
 * @param inc valore da aggiungere al numero registrati    
 */
void incrKeys(int inc);


/**
 * @function hash_create
 * @brief Crea una tabella hash 
 *
 * @return OP_OK           in caso di successo
 *         -1              in caso di errore mutex
 *         OP_FAIL         in caso di errore     
 *         OP_NICK_ALREADY se l'utente è già registrato 
 */
hash * hash_create();



/**
 * @function lista_addT
 * @brief Aggiunge un elemeneto in testa ad una lista 
 *
 * @param list      lista in cui aggiungere l'elemento 
 * @param el        elemento da aggiungere nella lista
 *
 */
void lista_addT(lista ** list,char * el);



/**
 * @function lista_addC
 * @brief Aggiunge un elemeneto in coda ad una lista 
 *
 * @param list      lista in cui aggiungere l'elemento 
 * @param el        elemento da aggiungere nella lista
 *
 */
void lista_addC(lista **list, char* el);




/**
 * @function mlista_addC
 * @brief Aggiunge un messaggio in coda nella history di un utente
 *
 * @param list           lista in cui aggiungere l'elemento 
 * @param msg        messaggio da aggiungere nella history
 * @param consegnat      valore di consegna del messaggio
 * @param sender         mittente dele messaggio
 * @param destinatario   destinatario del messaggio
 *
 */
void mlista_addC(lista **list,char*msg,int consegnat,char* sender,char* destinatario);



/**
 * @function lista_delT
 * @brief Elimina l'elemento in testa alla lista
 *
 * @param list  lista della quale eliminare la testa 
 *
 */
void lista_delT(lista ** list);



/**
 * @function mhash_insert
 * @brief Aggiunge un messaggio nella history di un utente 
 *
 * @param sender      nickname da aggiungere tra i registrati 
 * @param key         destinatario del messaggio
 * @param msg     messaggio 
 * @param consegnat   valore di consegna del messaggio
 *
 * @return OP_OK           in caso di successo
 *         -1              in caso di errore mutex
 *         OP_FAIL         in caso di errore     
 */
int mhash_insert( char* sender,char* key,char*msg,int consegnat);



/**
 * @function hash_registered_insert
 * @brief Aggiunge un utente nella tabella hash registrati
 *
 * @param key       nickname da aggiungere tra i registrati 
 * @param arg       file descriptor associato al nickname che porge la richiesta
 *
 * @return OP_OK           in caso di successo
 *         -1              in caso di errore mutex
 *         OP_FAIL         in caso di errore     
 *         OP_NICK_ALREADY se l'utente è già registrato 
 */
int hash_registered_insert( char* key,int arg);





/**
 * @function hash_member
 * @brief Controlla la presenza di un nickname nella tabella hash registrati
 *
 * @param ht        tabella hash nella quale aggiungere il nickname
 * @param sender    nickname da aggiungere tra i registrati 
 *
 * @return  0           in caso di successo
 *         -1              in caso di errore   
 */
int hash_member (hash *ht, char* key);








/**
 * @function hash_delete
 * @brief Elimina un nickname dalla tabella hash registrati
 *
 * @param key    nickname da eliminare dai registrati 
 *
 * @return OP_OK           in caso di successo
 *         -1              in caso di errore mutex
 *         OP_FAIL         in caso di errore     
 */
int hash_delete(void* key);



/**
 * @function mhash_delete
 * @brief Svuota la history di un nickname
 *
 * @param key   nickname di cui svuotare la history
 *
 * @return OP_OK           in caso di successo
 *         -1              in caso di errore mutex
 *         OP_FAIL         in caso di errore 
 *          OP_NICK_UNKNOWN se key non è presente nella tabella hash    
 */
int mhash_delete( void* key);




/**
 * @function hash_destroy
 * @brief Distrugge una tabella hash
 *
 * @param ht  tabella hash da distruggere
 *
 */
void hash_destroy(hash* ht);







#endif /* HASH_H_ */
