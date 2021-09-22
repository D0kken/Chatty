/**
 *   Stefano Russo 544341
 *
 *   stefanoruss97@gmail.com   
 *
 *  Si dichiara che il contenuto di questo file e' in ogni sua parte opera
 *    originale dell'autore
 */

#include<stdlib.h>

#include<config.h>
#include<errno.h>
#include<extern.h>
#include<connections.h>

 


int OP_Register(char* sender,int arg,int localcurr)
{
 

 if(localcurr<0 || localcurr>=msgmax || sender==NULL)
 	
 	   return OP_FAIL;

 int ret=0;

//inserisco sender nella struttura registrati
 ret=hash_registered_insert(sender,arg);

 if(ret==-1||ret>=OP_FAIL)

        return ret;

//se riesco ad inserire sender tra i registrati,lo inserisco anche tra i connessi 
 else
 {

 
//se la connessione non può avvenire,annullo anche la registrazione.
    if(conn->nconn==MaxConnections)
        {
            errno=ENOENT;
            
          

            hash_delete(sender);


            return OP_FAIL;
        }



if(pthread_mutex_lock(&connlock)!=0)
    {
        printf("connlock mutex errore");
        return -1;
    }


 
if(!IsConnesso(sender))
{

//trovo il primo posto libero nella struttura connessi
    while(conn->file_des[conn->ind]>0)
     {
        if(conn->ind==MaxConnections-1)
             conn->ind=0;

        else 
            conn->ind++;

        
     }


     if(conn->Conness[conn->ind]==NULL)
     
     conn->Conness[conn->ind]= calloc(1,strlen(sender)+1);
 	 
   	 fbackup(conn->Conness[conn->ind]);
 
     strncpy(conn->Conness[conn->ind],sender,strlen(sender));

     conn->file_des[conn->ind]=arg;
    
     conn->nconn++;

  }


}




 if(pthread_mutex_unlock(&connlock)!=0)
    {
        printf("connlock mutex errore");
        return -1;
    }




return OP_OK;
} 







int OP_Connect(char* sender,int arg,int localcurr)
{



    if(conn->nconn==MaxConnections || localcurr<0 || localcurr>=msgmax )
          {
            errno=ENOENT;
            return OP_FAIL;
          }




    if(hash_member (ht_registered,sender)==-1)
        {
           errno=EINVAL;
           return OP_NICK_UNKNOWN;
        }



    if(pthread_mutex_lock(&connlock)!=0)
        {
            printf("lock mutex errore");
                 return -1;
        } 



int k=0;
//controllo se l'utente è già connesso
for(k=0;k<MaxConnections;k++)
    {
        if(conn->Conness[k]!=NULL)
        {
            if(strcmp(conn->Conness[k],sender)==0)
                {
                                      
                    if(pthread_mutex_unlock(&connlock)!=0)
                    {
                            printf("connlock mutex errore");
                              return -1;
                    }

                    errno=EINVAL;
                    return OP_NICK_CONNECTED;
                }
       
        }

    }

 

//altrimenti trovo il primo posto disponibile nella struttura connessi 
 while(conn->file_des[conn->ind]>0)
    {
         if(conn->ind==MaxConnections-1)conn->ind=0;
         else conn->ind++;

    }

//e si inserisce il nickname
 if(conn->Conness[conn->ind]==NULL)
		 conn->Conness[conn->ind]=calloc(1,sizeof(MSGS->hdr.sender)+1);

 fbackup(conn->Conness[conn->ind]);
 
 strncpy(conn->Conness[conn->ind],sender,sizeof(MSGS->hdr.sender));
 
 conn->file_des[conn->ind]=arg;

 conn->nconn++;


  if(pthread_mutex_unlock(&connlock)!=0)
    {
        printf("connlock mutex errore");
        return -1;
    }



return OP_OK ;

} 








int OP_Posttxt( int localcurr )
{


int ret=0;    //valore che la funzione ritorna
int fd=0;     //conterrà il valore del file descriptor chi chi riceverà il messaggio
int consegnato=FALSE;  //indica se il messaggio è stato inviato o meno

if(MSGS[localcurr].data.hdr.len>=MaxMsgSize)
 	 return OP_MSG_TOOLONG;


char *buffer=calloc(1,MSGS[localcurr].data.hdr.len+1);
char *sender=calloc(1,strlen(MSGS[localcurr].hdr.sender)+1);
char *receiver=calloc(1,strlen(MSGS[localcurr].data.hdr.receiver)+1);

strncpy(sender,MSGS[localcurr].hdr.sender,strlen(MSGS[localcurr].hdr.sender));

strncpy(buffer,MSGS[localcurr].data.buf,MSGS[localcurr].data.hdr.len);

strncpy(receiver,MSGS[localcurr].data.hdr.receiver,strlen(MSGS[localcurr].data.hdr.receiver));

fbackup(receiver);
fbackup(sender);
fbackup(buffer);
       

if(localcurr<0 || localcurr>=msgmax || hash_member (ht_registered,receiver)==-1 )
  {
    return OP_NICK_UNKNOWN ;
  } 



if(pthread_mutex_lock(&connlock)!=0)
    {
     printf("connlock mutex errore");
      return -1;
    } 
  


//se il receiver è connesso,riceverà il messaggio in tempo reale
if(IsConnesso(receiver))
{

int k=0;
//scorro la lista connessi e ne ricavo il file descriptor che mi interessa
  for(k=0;k<MaxConnections;k++)
  {
    if(conn->file_des[k]>0)
    {
      if(strcmp(conn->Conness[k],receiver)==0)
    	{
         if(conn->file_des[k]>0)
        		 fd=conn->file_des[k];
         break;
     	}

    } 
      
    
  }

if(fd<0)
{
    errno=ENOENT;
    return OP_FAIL;
}





buffer[strlen(buffer)]=0;

setHeader(&MSGS[localcurr].hdr,TXT_MESSAGE,sender);

setData(&MSGS[localcurr].data,receiver,buffer,strlen(buffer));

sendRequest(fd,&MSGS[localcurr]);



if(statIncr(&chattyStats.ndelivered,1)==-1)            
        puts("errore ndelivered");


 consegnato=TRUE;


}

if(pthread_mutex_unlock(&connlock)!=0)
    {
     printf("connlock mutex errore");
       return -1;
    } 


//in ogni caso inserisco il messaggio nella history dei messagi relativi al receiver

ret=mhash_insert(sender,receiver,buffer,consegnato);

    

        
return ret;

}





int OP_Posttxtall( int localcurr )
{

if(localcurr<0 || localcurr>=msgmax ||ht_registered->nkeys==1)
    return OP_FAIL;

char *sender=calloc(1,sizeof(MSGS->hdr.sender)+1);

int len=MSGS[localcurr].data.hdr.len;
int htlock=0;   //indica se htrlock è acquisita
int clock=0;    //indica se htmlock è acquisita

int k=0;  //indici usati nei for
int i=0;

int fd=0;   //valore del file descriptor di chi riceverà il messaggio
int consegnato=FALSE; //indica se il messaggio è stato consegnato o meno
int ret=0;  //valore di ritorno della funzione

fbackup(sender);

lista* curr;


strncpy(sender,MSGS[localcurr].hdr.sender,sizeof(MSGS->hdr.sender));


if(strlen(MSGS[localcurr].data.buf)==0)
 {  
    return  OP_FAIL ;
 }


   if(len>MaxMsgSize)
     return  OP_MSG_TOOLONG;

 



  if(pthread_mutex_lock(&htrlock)!=0)
    {
        printf("htrlock mutex errore");
        return -1;
    } 


htlock=1;

     if(pthread_mutex_lock(&connlock)!=0)
    {
        printf("connlock mutex errore");
        return -1;
    }


clock=1;

//scansiono tutti i registrati in questo momento e invio loro il messaggio nella stessa modalità di OP_Postxt
for(i=0;i<MAX_CELLE;i++)

{

    if(ht_registered->cella[i]!=NULL)
    {
 
        for (curr=ht_registered->cella[i]; curr != NULL; curr=curr->next)
         {
       
             if ( curr->key && (strcmp(curr->key,sender)!=0) )
                {
                  
                    if(IsConnesso(curr->key))
                        {

                            for(k=0;k<MaxConnections;k++)
                                {
                                  if(conn->Conness[k]!=NULL )
                                    {  
                                        if(strcmp(conn->Conness[k],curr->key)==0)
                                            {
                                            if(conn->file_des[k]>0)
                                            	  fd=conn->file_des[k];

                                            setHeader(&MSGS[localcurr].hdr,TXT_MESSAGE,sender);
                                        
                                            setData(&MSGS[localcurr].data,curr->key,MSGS[localcurr].data.buf,len);
                                          
                                         
                                            sendRequest(fd,&MSGS[localcurr]);
                                   
                                   
                                              if(statIncr(&chattyStats.ndelivered,1)==-1)         
                                                    puts("errore ndelivered");

                                                        consegnato=TRUE;

                                              break;
                                          }

                                    }
                                
    
                                }

                               



if(htlock)
{

                    if(pthread_mutex_unlock(&htrlock)!=0)
                        {
                        printf("htrlock mutex errore");
                             return -1;
                        } 
  htlock=0;
}

if(clock)
{
                    if(pthread_mutex_unlock(&connlock)!=0)
                        {
                            printf("connlock mutex errore");
                                 return -1;
                        }
clock=0;
}





                        }

                    
                        
                        ret=mhash_insert(sender,curr->key,MSGS[localcurr].data.buf,consegnato);

                        if(ret==-1||ret>=OP_FAIL)
                             return ret;


                            consegnato=FALSE;

        

                }     
                 

    
        }

    }

 } 



if(htlock)
{

                    if(pthread_mutex_unlock(&htrlock)!=0)
                        {
                        printf("htrlock mutex errore");
                             return -1;
                        } 
  
}

if(clock)
{
                    if(pthread_mutex_unlock(&connlock)!=0)
                        {
                            printf("connlock mutex errore");
                                 return -1;
                        }

}


return ret;
}







 int OP_Prevmsgs(char* sender,int localcurr,int fd)
{// sender è chi ha mandato la richiesta di prevmsgs


lista* curr;
int hash_val=mfhash(sender);

if(pthread_mutex_lock(&ht_msgs->trab[hash_val])!=0)
              {
            printf("htmlock mutex errore");
                 return -1;
              } 
  
   

if(ht_msgs->nmes[hash_val]<=0){     // se non ci sono messaggi per lui ritorno fail
 
if(pthread_mutex_unlock(&ht_msgs->trab[hash_val])!=0)
              {
            printf("htmlock mutex errore");
                 return -1;
              } 
  
  return OP_FAIL;
}


char *intero=calloc(1,sizeof(ht_msgs->nmes[hash_val]));
snprintf(intero,sizeof(ht_msgs->nmes[hash_val]),"%c",ht_msgs->nmes[hash_val]);


intero[strlen(intero)]=0;


   //se ci sono messaggi gli mando ok + il numero di messaggi
setHeader(&MSGS[localcurr].hdr,OP_OK,ht_msgs->cella[hash_val]->key);
setData(&MSGS[localcurr].data,NULL,intero,strlen(intero)+1);
sendRequest(fd,&MSGS[localcurr]);

//mando i messaggi della history
      for (curr=ht_msgs->cella[hash_val]; curr != NULL;curr=curr->next)
    {
           if (curr->key!=NULL && curr->messaggio!=NULL && strcmp(curr->destinatario,sender)==0 )
          {
            
            setHeader(&MSGS[localcurr].hdr,TXT_MESSAGE,curr->key);
          
            setData(&MSGS[localcurr].data,curr->destinatario,curr->messaggio,strlen(curr->messaggio));//per ogni messaggio per lui gli mando data
           
            sendRequest(fd,&MSGS[localcurr]);                                                                           //e setto il messaggio come consegnato

           
                          //se il contatore messaggio consegnato non è settato,lo setto a 0
              if(curr->consegnato==FALSE) //e aumento il numero di messaggi consegnati di 1 e decremento 
              {           //di 1 il numero di messaggi non consegnati
              curr->consegnato=TRUE;
              if(statIncr(&chattyStats.ndelivered,1)==-1)
                   puts("errore ndelivered");
              if(statIncr(&chattyStats.nnotdelivered,-1)==-1)
                puts("errore nnotdelivered");

              }




    
         }

      }

       
if(pthread_mutex_unlock(&ht_msgs->trab[hash_val])!=0)
              {
            printf("htmlock mutex errore");
                 return -1;
              } 
  


free(intero);

return OP_OK;
}






int OP_postfile(int arg,int localcurr,char* nomefile)
{



if(localcurr<0 || localcurr>=msgmax )
    return OP_FAIL;





if(MSGS[localcurr].data.hdr.len>(MaxFileSize*1000))
    return OP_MSG_TOOLONG;



int consegnato=TRUE;


if(hash_member (ht_registered,MSGS[localcurr].data.hdr.receiver)==-1)
{
    puts("[-]receiver not member");
    return OP_NICK_UNKNOWN;
}





int ret=0;
int fd=0;     

char *receiver=calloc(1,strlen(MSGS[localcurr].data.hdr.receiver)+1); 

strncpy(receiver,MSGS[localcurr].data.hdr.receiver,strlen(MSGS[localcurr].data.hdr.receiver));

receiver[strlen(receiver)]=0;

char *sender=calloc(1,strlen(MSGS[localcurr].hdr.sender)+1); 

strncpy(sender,MSGS[localcurr].hdr.sender,strlen(MSGS[localcurr].hdr.sender));

sender[strlen(sender)]=0;



  if(pthread_mutex_lock(&connlock)!=0)
    {
        printf("connlock mutex errore");
        return -1;
    }

//se il receiver è connesso riceverà il messaggio in tempo reale
if(IsConnesso(receiver))
 {
   int k=0;
//ottengo il file descriptor del receiver
  for(k=0;k<MaxConnections;k++)
   {
      if(conn->file_des[k]>0)
      {
          if(strcmp(conn->Conness[k],receiver)==0)
          {
             if(conn->file_des[k]>0)
           		  fd=conn->file_des[k];
             break;
          }

     }
      
    
    
   }





setHeader(&MSGS[localcurr].hdr,FILE_MESSAGE,sender);

setData(&MSGS[localcurr].data,receiver,nomefile,strlen(nomefile));

sendRequest(fd,&MSGS[localcurr]);


consegnato=TRUE;

if(statIncr(&chattyStats.ndelivered,1)==-1)
        puts("errore ndelivered");



}


if(pthread_mutex_unlock(&connlock)!=0)
    {
        printf("connlock mutex errore");
        return -1;
    }

//in ogni caso inserisco il messaggio di notifica di ricezione del file nella history dei messaggi relativi al receiver
ret = mhash_insert(MSGS[localcurr].hdr.sender,receiver,nomefile,consegnato);
 
           
//rendo disponibile l'indice localcurr-1 di MSGS per un possibile riutilizzo
            free(receiver);
            free(sender);
                    
            



if(statIncr(&chattyStats.nfilenotdelivered,1)==-1)
        
            puts("errore nfilenotdelivered");
            
            


return ret;

}





int OP_GetFile(int fd,int localcurr)
{


if(localcurr<0 || localcurr>=msgmax )
    return OP_FAIL;



char nomefile[MSGS[localcurr].data.hdr.len+1];


strncpy(nomefile,MSGS[localcurr].data.buf,MSGS[localcurr].data.hdr.len);




  FILE *file = fopen(nomefile, "r");
  fseek(file, 0, SEEK_END);
  // Vado fino in fondo al file per sapere quanto è lungo
  long fsize = ftell(file);
  // Torno all'inizio per leggerlo interamente
  fseek(file, 0, SEEK_SET);

  char *lettura = calloc(1,fsize + 1);
  if(lettura == NULL) {
    return -1;
  }
  fread(lettura, fsize, 1, file);
  fclose(file);

  lettura[fsize] = 0;





setHeader(&MSGS[localcurr].hdr,OP_OK,MSGS[localcurr].hdr.sender);

setData(&MSGS[localcurr].data,MSGS[localcurr].data.hdr.receiver,lettura,strlen(lettura));

sendRequest(fd,&MSGS[localcurr]);


if(statIncr(&chattyStats.nfilenotdelivered,-1)==-1)
         puts("errore nfilenotdelivered");

if(statIncr(&chattyStats.nfiledelivered,1)==-1)
         puts("errore nfiledelivered");     



free(lettura);

return OP_OK;
}






int OP_disconnetti(char* nick,int sd)
{

if(nick==NULL && sd<=0 )
    return OP_FAIL;


int i=0;



  if(pthread_mutex_lock(&connlock)!=0)
    {
        printf("connlock mutex errore");
        return -1;
    }


for(i=0;i<MaxConnections;i++)
  {


if(conn->file_des[i]==sd)
	{

 			conn->Conness[i]=NULL;

            conn->file_des[i]=0;
                                    
            conn->nconn--;  
        
            break;

	}



    if(conn->Conness[i])
     {
        if(strcmp(conn->Conness[i],nick)==0)
         {
       
                
            conn->Conness[i]=NULL;
           
            conn->file_des[i]=0;
                      
           	conn->nconn--;  

            break;




          }           
 
      }
 
  }


 


  if(pthread_mutex_unlock(&connlock)!=0)
    {
        printf("connlock mutex errore");
        return -1;
    }




return OP_OK;

}