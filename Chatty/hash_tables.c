/**
 *   Stefano Russo 544341
 *
 *   stefanoruss97@gmail.com   
 *
 *  Si dichiara che il contenuto di questo file e' in ogni sua parte opera
 *    originale dell'autore
 */

#include <pthread.h>
#include <errno.h>
#include <stdlib.h>

#include <hash_tables.h>
#include <config.h>
#include <extern.h>

//funzione hash che calcola la posizione nella hash table registrati
int fhash(const char *string)
{

int i=0;
int ncelle=5021;
int key=0; 
for(i=0;i<strlen(string);i++)

           key+=string[i];


return key % ncelle;
}



//funzione hash che calcola la posizione nella hash table messaggi
int mfhash(const char *string)
{
  int i=0;
  int libero=0;
  int trovato=0;
  int presente=0;

while( i<MAX_CELLE && !presente )//se il nome è già presente allora  segno quell'indice per inserire
{
  if(!trovato)
       if(ht_msgs->cella[i]==NULL)
      {
        libero=i;
        trovato=1;
      }//trovo la prima cella libera e fermo le mie ricerche
    

   if(ht_msgs->cella[i] !=NULL)//se la cella è occupata....
   {
         if(strcmp(ht_msgs->cella[i]->destinatario,string)==0)//se la occupa lo stesso nickname
        {

         
           libero=i;//inserisco quà
           presente=1;//termino la ricerca
        }
    }
     i++;

  }

return libero;
}





void incrKeys(int inc){


if(pthread_mutex_lock(&htrlock)!=0)
  {
    printf("htrlock mutex errore");
      return ;
  }



ht_registered->nkeys+=inc;

if(pthread_mutex_unlock(&htrlock)!=0)
  {
    printf("htrlock mutex errore");
      return ;
  }


}






void lista_addT(lista ** list,char * el)
{

lista* new=calloc(1,sizeof(lista));

new->key=calloc(1,1+strlen(el));

strcpy(new->key,el);

new->next=*list;

*list=new;



}




void lista_addC(lista **list, char* x)
{

lista *curr,*new=malloc(sizeof(lista));

  new->key=x;
  new->next=NULL;

  if(*list==NULL)
	*list=new;

  else
     {
     curr=*list;

     while(curr->next!=NULL)
	      curr=curr->next;

      curr->next=new;
     }

}





void lista_delT(lista ** list)
{

lista* curr=*list;

*list=curr->next;

if(curr->key!=NULL)
	free(curr->key);

if(curr->messaggio!=NULL)
	free(curr->messaggio);

if(curr->destinatario!=NULL)
	free(curr->destinatario);

free(curr);

}










void mlista_addC(lista **list,char*messaggio,int consegnat,char* sender,char* destinatario)
{


 lista *curr,*new=malloc(sizeof(lista));

 new->key=calloc(1,2+strlen(sender)); //chi ha mandato il messaggio al destinatario
 new->destinatario=calloc(1,2+strlen(destinatario));//chi ha ricevuto il messagigo dal sender
 new->messaggio=calloc(1,strlen(messaggio)+2);
 
 strncpy(new->messaggio,messaggio,strlen(messaggio));

 strncpy(new->destinatario,destinatario,strlen(destinatario));
 
 strncpy(new->key,sender,strlen(sender));

new->messaggio[strlen(messaggio)]=0;
new->key[strlen(sender)]=0;
new->destinatario[strlen(destinatario)]=0;

  new->next=NULL;
  new->consegnato=consegnat;//consegnato viene settato a 0 se è stato già consegnato su posttxt perchè l'utente è connesso,altrimenti viene settato a 1 perchè è offline
 
 if(*list==NULL)
  *list=new;

 else
   {
     curr=*list;

     while(curr->next!=NULL)
      	  curr=curr->next;

      curr->next=new;
   }


if(consegnat==FALSE)
  if( statIncr(&chattyStats.nnotdelivered,1)==-1)
  	  puts("errore nnotdelivered");           //se consegnat è 1 vuol dire che il messaggio aggiunto non è cosnegnato quindi devo incrementare 

}





 int mhash_insert(char* sender,char* key,char*msg,int consegnat)
{


int hash_val=0;;


    if( key==NULL) 
      {
         errno=EINVAL;
         return OP_FAIL ;
      }





  if(pthread_mutex_lock(&htmlock)!=0)
  {
    printf("htrlock mutex errore");
      return -1;
  }

hash_val = mfhash(key);




//se la history e piena elimino il messaggio più vecchio
if(ht_msgs->nmes[hash_val]>=MaxHistMsgs)
{

  lista_delT(&ht_msgs->cella[hash_val]);
  ht_msgs->nmes[hash_val]--;


}

mlista_addC(&ht_msgs->cella[hash_val],msg,consegnat,sender,key); //key== destinatario del messaggio

ht_msgs->nmes[hash_val]++;



  if(pthread_mutex_unlock(&htmlock)!=0)
  {
    printf("htmlock mutex errore");
      return -1;
  }



    return OP_OK;
}





 int mhash_delete( void* key)
{


if(key==NULL)
        return OP_FAIL;

int hash_val=0;

    
hash_val = mfhash(key);



  if(pthread_mutex_lock(&ht_msgs->trab[hash_val])!=0)
  {
    printf("htmlock mutex errore");
      return -1;
  }



while(ht_msgs->cella[hash_val]!=NULL)
  {
      lista_delT(&ht_msgs->cella[hash_val]);
}





  if(pthread_mutex_unlock(&ht_msgs->trab[hash_val])!=0)
  {
    printf("htmlock mutex errore");
      return -1;
  }


return OP_OK;
}





void hash_destroy(hash* ht)
{

int i=0;

lista* curr;

lista* succ=NULL;


for(i=0;i<MAX_CELLE;i++)
   {
    
   	pthread_mutex_destroy(&ht->trab[i]);

    for (curr=ht->cella[i]; curr != NULL;)
		{ 
        succ=curr->next;
		lista_delT(&ht->cella[i]);
      	curr=succ;
   
      
	    }	
	
    }
    
    
free(ht);
   

}





hash * hash_create()
{   

 hash *ht = calloc(1, sizeof(hash));

for(int i=0;i<MAX_CELLE;i++)

if(pthread_mutex_init(&ht->trab[i],NULL)!=0 )
	{
		printf("errore inizializzazione mutex");
  		return NULL;
    }


  
    if(ht==NULL) 
      {
		errno=ENOENT;
	    return NULL;
     }
  
   
    memset(ht, 0, sizeof(hash));
   
  return ht;
}





int hash_member (hash *ht, char* key)
{


lista* curr;
int hash_val=0;
hash_val = fhash(key);

if(pthread_mutex_lock(&ht->trab[hash_val])!=0)
					    {
 	 					printf("htrlock mutex errore");
	   						 return -1;
				     	} 


    for (curr=ht->cella[hash_val]; curr != NULL; curr=curr->next)
		{
                if ( strcmp(curr->key,key)==0 )
                {
			 


if(pthread_mutex_unlock(&ht->trab[hash_val])!=0)
					    {
 	 					printf("htrlock mutex errore");
	   						 return -1;
				     	} 
  


                         return 0;   
                }   
        }


if(pthread_mutex_unlock(&ht->trab[hash_val])!=0)
					    {
 	 					printf("htrlock mutex errore");
	   						 return -1;
				     	} 
  

return -1;
}





int hash_registered_insert( char* key,int arg)
{


lista *curr;
int hash_val=0;

    if( key==NULL) 
      {
         errno=EINVAL;
         return OP_FAIL ;
      }




hash_val = fhash(key);

if(pthread_mutex_lock(&ht_registered->trab[hash_val])!=0)
  {
    printf("htrlock mutex errore");
      return -1;
  }




    for (curr=ht_registered->cella[hash_val]; curr != NULL; curr=curr->next)
      {
        if(curr->key!=NULL)
         {
               if ( strcmp(curr->key,key)==0)//già registrato
               {
                  errno=EINVAL;
                            

                if(pthread_mutex_unlock(&ht_registered->trab[hash_val])!=0)
                   {
                        printf("htrlock mutex errore");
                         return -1;
                   }

                return  OP_NICK_ALREADY;
              }
              
          }
        
     }   
      
       
lista_addT(&ht_registered->cella[hash_val],key);


 ht_registered->cella[hash_val]->fd=arg;  


if(pthread_mutex_unlock(&ht_registered->trab[hash_val])!=0)
  {
    printf("htrlock mutex errore");
      return -1;
  }


incrKeys(1);//aggiunge il numero passato al numero di chiavi nell'hash table registrati


  return OP_OK;
}















 int hash_delete(void* key)
{


lista *curr, *prec;
 int hash_val=0;


if(!key){
      errno=EINVAL;
        return OP_FAIL;
      }
hash_val = fhash(key);



  if(pthread_mutex_lock(&ht_registered->trab[hash_val])!=0)
  {
    printf("htrlock mutex errore");
      return -1;
  }



prec = NULL;
    
for (curr=ht_registered->cella[hash_val]; curr != NULL; curr = curr->next) 
   {//cerco il nick da cancellare
        
       if ( strcmp(curr->key,key)==0) 
     {

            if (prec == NULL) 
                 
                ht_registered->cella[hash_val] = curr->next;
            
            else 
                prec->next = curr->next;
              

            incrKeys(-1);
            
       
            free(curr->key);                
            free(curr);


            if(pthread_mutex_unlock(&ht_registered->trab[hash_val])!=0)
  {
    printf("htrlock mutex errore");
      return -1;
  }
            
          return OP_OK;

         }



   prec = curr;

     
   }



  if(pthread_mutex_unlock(&ht_registered->trab[hash_val])!=0)
  {
    printf("htrlock mutex errore");
      return -1;
  }


return OP_NICK_UNKNOWN;
}


