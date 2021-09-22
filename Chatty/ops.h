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

#ifndef OPS_H_
#define OPS_H_


/**
 * @file  ops.h
 * @brief Contiene i codici delle operazioni di richiesta e risposta
 */
#define MAX_NAME_LENGTH 32

typedef enum {

    /* ------------------------------------------ */
    /*      operazioni che il server deve gestire */
    /* ------------------------------------------ */
    REGISTER_OP      = 0,   /// richiesta di registrazione di un ninckname

    CONNECT_OP       = 1,   /// richiesta di connessione di un client
    POSTTXT_OP       = 2,   /// richiesta di invio di un messaggio testuale ad un nickname o groupname
    POSTTXTALL_OP    = 3,   /// richiesta di invio di un messaggio testuale a tutti gli utenti 
    POSTFILE_OP      = 4,   /// richiesta di invio di un file ad un nickname o groupname
    GETFILE_OP       = 5,   /// richiesta di recupero di un file
    GETPREVMSGS_OP   = 6,   /// richiesta di recupero della history dei messaggi
    USRLIST_OP       = 7,   /// richiesta di avere la lista di tutti gli utenti attualmente connessi
    UNREGISTER_OP    = 8,   /// richiesta di deregistrazione di un nickname o groupname
    DISCONNECT_OP    = 9,   /// richiesta di disconnessione

    CREATEGROUP_OP   = 10,  /// richiesta di creazione di un gruppo
    ADDGROUP_OP      = 11,  /// richiesta di aggiunta ad un gruppo
    DELGROUP_OP      = 12,  /// richiesta di rimozione da un gruppo


    /* NOTA: la richiesta di cancellazione di un gruppo e' lasciata come task opzionale */

    /* 
     * aggiungere qui eltre operazioni che si vogliono implementare 
     */

    /* ------------------------------------------ */
    /*    messaggi inviati dal server             */
    /* ------------------------------------------ */

    OP_OK           = 20,  // operazione eseguita con successo    
    TXT_MESSAGE     = 21,  // notifica di messaggio testuale
    FILE_MESSAGE    = 22,  // notifica di messaggio "file disponibile"

    OP_FAIL         = 25,  // generico messaggio di fallimento
    OP_NICK_ALREADY = 26,  // nickname o groupname gia' registrato
    OP_NICK_UNKNOWN = 27,  // nickname non riconosciuto
    OP_MSG_TOOLONG  = 28,  // messaggio con size troppo lunga
    OP_NO_SUCH_FILE = 29,  // il file richiesto non esiste
    OP_NICK_CONNECTED = 30,  // nickname  gia' connesso


    /* 
     * aggiungere qui altri messaggi di ritorno che possono servire 
     */
    FREE_BLOCK = 33, //blocco libero su MSGS

    OP_END          = 100 // limite superiore agli id usati per le operazioni

} op_t;
    




/**
 * @function OP_Register
 * @brief Aggiunge un nickname tra i nickname registrati 
 *
 * @param sender    nickname da aggiungere tra i registrati 
 * @param arg       file descriptor associato al nickname che porge la richiesta
 * @param localcurr indice di MSGS contenente la richiesta 
 *
 * @return OP_OK           in caso di successo
 *         -1              in caso di errore mutex
 *         OP_FAIL         in caso di errore     
 *         OP_NICK_ALREADY se l'utente è già registrato 
 */
int OP_Register   (char* sender,int arg,int localcurr);





/**
 * @function OP_Connect
 * @brief Aggiunge un nickname tra i nickname connessi 
 *
 * @param sender    nickname da aggiungere tra i connessi 
 * @param arg       file descriptor associato al nickname che porge la richiesta
 * @param localcurr indice di MSGS contenente la richiesta 
 *
 * @return OP_OK             in caso di successo
 *         -1                in caso di errore mutex
 *         OP_FAIL           in caso di errore     
 *         OP_NICK_UNKNOWN   se l'utente non è registrato 
 *         OP_NICK_CONNECTED se l'utente è gia connesso
 */
int OP_Connect    (char* sender,int arg,int localcurr);






/**
 * @function OP_Posttxt
 * @brief Manda un messaggio testuale all'utente richiesto
 *
 * @param localcurr indice di MSGS contenente la richiesta 
 *
 * @return OP_OK             in caso di successo
 *         -1                in caso di errore mutex
 *         OP_FAIL           in caso di errore     
 *         OP_MSG_TOOLONG    se il messaggio testuale è troppo lungo 
 *         OP_NICK_UNKNOWN   se l'utente non è registrato
*/
int OP_Posttxt    (int  localcurr );







/**
 * @function OP_Posttxtall
 * @brief Manda un messaggio testuale a tutti gli utenti registrati
 *
 * @param localcurr indice di MSGS contenente la richiesta 
 *
 * @return OP_OK             in caso di successo
 *         -1                in caso di errore mutex
 *         OP_FAIL           in caso di errore     
 *         OP_MSG_TOOLONG    se il messaggio testuale è troppo lungo 
 *         OP_NICK_UNKNOWN   se l'utente non è registrato
*/
int OP_Posttxtall (int  localcurr );






/**
 * @function OP_Prevmsgs
 * @brief Manda al client la sua history di messaggi
 *
 * @param sender    nickname che richiede la propria history messaggi 
 * @param localcurr indice di MSGS contenente la richiesta 
 * @param fd       file descriptor associato al nickname che porge la richiesta
 *
 * @return OP_OK           in caso di successo
 *         -1              in caso di errore mutex
 *         OP_FAIL         in caso di errore     
 *         OP_NICK_ALREADY se l'utente è già registrato 
 */
int OP_Prevmsgs(char* sender,int localcurr,int fd);




/**
 * @function OP_postfile
 * @brief Scarica il file mandato dall'utente e notifica il receiver che quel file a lui destinato è pronto a essere scaricato 
 *
 * @param arg       file descriptor associato al nickname che porge la richiesta
 * @param localcurr indice di MSGS contenente la richiesta 
 * @param nomefile  nome del file da inviare
 * @return OP_OK             in caso di successo
 *         -1                in caso di errore mutex
 *         OP_FAIL           in caso di errore     
 *         OP_MSG_TOOLONG    se il messaggio testuale è troppo lungo 
 *         OP_NICK_UNKNOWN   se l'utente non è registrato
*/
int OP_postfile   (int  arg,int localcurr ,char*nomefile);





/**
 * @function readFile
 * @brief Manda il contenuto del file richiesto,all'utente
 *
 * @param fd       file descriptor associato al nickname che porge la richiesta
 * @param localcu indice di MSGS contenente la richiesta 
 *
 * @return OP_OK             in caso di successo
 *         -1                in caso di errore mutex
 *         OP_FAIL           in caso di errore     
 *         OP_MSG_TOOLONG    se il messaggio testuale è troppo lungo 
 *         OP_NO_SUCH_FILE   se il file richiesto non è raggiungibile o non esiste
*/
int OP_GetFile   (int  fd,int localcu  );





//int OP_disconnetti   (int  localcurr,int fd);
/**
 * @function Rimuove il nickname dalla struttura contentente i nickname registrati
 * @brief Apre una connessione AF_UNIX verso il server 
 *
 * @param nick    nickname da eliminare dai registrati 
 * @param localcurr indice di MSGS contenente la richiesta 
 *
 * @return OP_OK           in caso di successo
 *         -1              in caso di errore mutex
 *         OP_FAIL         in caso di errore     
 */
int OP_disconnetti(char*nick,int localcurr);





#endif /* OPS_H_ */

