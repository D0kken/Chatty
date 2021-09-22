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

#include<stdio.h>
#include<unistd.h>
#include<assert.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<stdint.h>
#include<sys/types.h> 
#include<sys/socket.h>
#include<sys/un.h>
#include<errno.h>
#include<signal.h>
#include<sys/time.h>

#include<pthread.h>
#include<connections.h>
#include<config.h>



 char* DirName=NULL;


 int i=0;//indice dell'array backyp
 
 char* backup[MAX_BACKUP];//array per salvare i puntatori da liberare in chiusura
 
 pthread_mutex_t backlock;//mutex per l'accesso a backup




 int fbackup( char *string)
{


if(pthread_mutex_lock(&backlock)!=0)
              {
            printf("backlock mutex errore");
                 return 0;
           
             } 

if(i>=MAX_BACKUP)
  i=0;

   

backup[i]=string;
i++;


if(pthread_mutex_unlock(&backlock)!=0)
              {
            printf("lock mutex errore");
                 return 0;
              } 
  

return 1;
}






static inline int readn(long fd, void *buf, size_t size) {
    if(buf == NULL) 
          return 1;
  
    size_t left = size;
    int r;
    char *bufptr = (char*)buf;
    while(left>0) {
  if ((r=read((int)fd ,bufptr,left)) == -1) {
      if (errno == EINTR) continue;
      return -1;
  }
  if (r == 0) return 0;  
        left    -= r;
  bufptr  += r;
    }
    return size;
}


static inline int writen(long fd, void *buf, size_t size) {
    if(buf == NULL) 
          return 1;
  
    size_t left = size;
    int r;
    char *bufptr = (char*)buf;
    while(left>0) {
  if ((r=write((int)fd ,bufptr,left)) == -1) {
      if (errno == EINTR) continue;
      return -1;
  }
  if (r == 0) return 0;  
        left    -= r;
  bufptr  += r;
    }
    return 1;
}




int openConnection(char* path, unsigned int ntimes, unsigned int secs){

int fc=0,t=0;

struct sockaddr_un sa;
strncpy(sa.sun_path,path,UNIX_PATH_MAX);
sa.sun_family=AF_UNIX;
fc=socket(AF_UNIX,SOCK_STREAM,0);


while(connect(fc,(struct sockaddr*)&sa,sizeof(sa))==-1)
{

   if(t<ntimes)t++; 

   else return -1;

   sleep(secs);

}


return fc;

}


int readHeader(int connfd, message_hdr_t *hdr)
{
errno=0;
int a=0;													    //variabile intera per contenere l'intero 
int ret=0;														//valore di lettura 


char *buffer=malloc(sizeof(hdr->op)+sizeof(hdr->sender));		//buffer che contiene il messaggio ricevuto
char *sender=malloc(sizeof(hdr->sender));						        //sender che contiene sender
char *buffer2=malloc(sizeof(hdr->op));							        //buffer che contiene il numero formato char		
op_t op;														                        //variabile op_t che conterrà il valore op 



if(!connfd)
	{

	 errno=EINVAL;
        return -1;
	}


 if ((ret = readn( connfd , buffer, sizeof(hdr->op)+sizeof(hdr->sender))) <= 0)   //leggo sender e op
    {   
    
        return ret;

     }
		

else

	{

//converto il numero da char a int
  char *ptr;

	strncpy(buffer2,buffer,sizeof(hdr->op));
	
	a=strtol(buffer2,&ptr,10);																
	
	op=a;
	

	strncpy(sender,buffer+sizeof(hdr->op),sizeof(hdr->sender));

  sender[strlen(sender)]=0;

	

  setHeader(hdr,op,sender);

	}

if(!fbackup(sender))return 0;

free(buffer);

free(buffer2);


return ret;
}








int readData(int fd, message_data_t *data){

int ret=0;									                     //valore di lettura
unsigned int len=0;								               //lunghezza messaggio							

char *buffer=malloc(sizeof(data->hdr.receiver)+sizeof(data->hdr.len)+1);			                   //buffer che conterrà ll'header del messaggio ricevuto
char *receiver=malloc(sizeof(data->hdr.receiver)+1);
char *buf;                                        //buffer che conterrà il messaggio vero e proprio
char *bufferlen=malloc(sizeof(data->hdr.len)+1);  //buffer che conterrà la lunghezza del messaggio 



if(!fd)
{
	errno=EINVAL;
     return -1;
}

//leggo header di data del messaggio
 if ((  ret = readn( fd , buffer,sizeof(data->hdr.receiver)+sizeof(data->hdr.len))   ) <= 0) 
     {      
        
         return 0 ;
      }
                     

else
{

 
strncpy(receiver,buffer,sizeof(data->hdr.receiver));

receiver[strlen(receiver)]=0;


strncpy(bufferlen,buffer+sizeof(data->hdr.receiver),sizeof(data->hdr.len));

//trasformo il valore char in unsigned int  e lo inserisco su len													
sscanf(bufferlen,"%u",&len);

	

buf=malloc(len+1);

//leggo il messaggio vero e proprio
readn(fd,buf,len);

buf[len]=0;


setData(data,receiver,buf,len);
}


return ret; 
}










int readMsg(int fd, message_t *msg)
{ 


int ret=0;

ret=readHeader(fd, &(msg->hdr));


if (ret ==0)
		{
	   
	
		return 0;
                
		}

else if(ret<0)
	{
		perror("readHeader");	
		return -1;
	}



return readData(fd, &(msg->data));

}











int sendRequest(long fd, message_t *msg)
{

//rendo il valre di msg->hdr.op un char da poter inviare
char *intero=malloc(sizeof(msg->hdr.op));
snprintf(intero,sizeof(msg->hdr.op),"%d",msg->hdr.op);

//buffer conterrà l'intero messaggio da inviare
char *buffer=malloc(sizeof(msg->hdr.sender)+sizeof(msg->hdr.op)+sizeof(msg->data.hdr.receiver)+sizeof(msg->data.hdr.len)+msg->data.hdr.len+1);

char *databuf=msg->data.buf;
strncpy(buffer,intero,sizeof(msg->hdr.op));
strncpy(buffer+sizeof(msg->hdr.op),msg->hdr.sender,sizeof(msg->hdr.sender));

        
int n=sizeof(msg->hdr.sender)+sizeof(msg->hdr.op);




char *len=malloc(sizeof(msg->data.hdr.len)+1);
snprintf(len,msg->data.hdr.len,"%u",msg->data.hdr.len);


if(msg->data.hdr.receiver!=NULL)
strncpy(buffer+n,msg->data.hdr.receiver,sizeof(msg->data.hdr.receiver));


if(msg->data.buf!=NULL)
  {
 	strncpy(buffer+n+sizeof(msg->data.hdr.receiver),len,sizeof(msg->data.hdr.len));

  }


int N=n+sizeof(msg->data.hdr.receiver)+sizeof(msg->data.hdr.len);

free(intero);
free(len);

buffer[N]=0;



if(!fbackup(buffer))return 0;
   
writen(fd,buffer,N);

return writen(fd,databuf,msg->data.hdr.len);

}



int sendHeader(long fd, message_hdr_t *msg)
{

//rendo il valre di msg->op un char da poter inviare
char *intero=malloc(sizeof(msg->op));
snprintf(intero,sizeof(msg->op),"%d",msg->op);


char *buffer=malloc(sizeof(msg->sender)+sizeof(msg->op));


strncpy(buffer,intero,sizeof(msg->op));
strncpy(buffer+sizeof(msg->op),msg->sender,sizeof(msg->sender));

        
int n=sizeof(msg->sender)+sizeof(msg->op);




free(intero);

if(!fbackup(buffer))return 0;

return writen(fd,buffer,n);


}





int sendData(long fd, message_data_t *msg)
{
  int error=1;

int len=sizeof(msg->hdr.len)+2;

int l=snprintf(NULL,0,"%u",msg->hdr.len);
char *intero=calloc(1,l+1);
snprintf(intero,l+1,"%u",msg->hdr.len);

char *buffer=calloc(1,(sizeof(msg->hdr.receiver)+len+1));



strncpy(buffer,msg->hdr.receiver,sizeof(msg->hdr.receiver));

strncpy(buffer+sizeof(msg->hdr.receiver),intero,len);



//mando header di data 
writen(fd,buffer,sizeof(msg->hdr.receiver)+len);



//mando il messaggio vero e proprio
writen( fd, msg->buf, msg->hdr.len);


if(!fbackup(buffer))error=0;

free(intero);



return error;

}



int HeaderMsgs(char* fd, message_hdr_t *hdr)
{

int N=sizeof(hdr->op)+sizeof(hdr->sender);
char *buffer=malloc(sizeof(hdr->sender)+sizeof(hdr->op));   //buffer che conterrà l'header del messaggio
op_t op;
char *sender=malloc(sizeof(hdr->sender));
char *buffer2=malloc(sizeof(hdr->op));
int a=0;




if(!fbackup(buffer))return -1;
if(!fbackup(sender))return -1;
if(!fbackup(buffer2))return -1;



if(fd==NULL)
	{

	 errno=EINVAL;
        return -1 ;
	}

 char *ptr;


strncpy(buffer2,fd,sizeof(hdr->op));


a=strtol(buffer2,&ptr,10);	

op=a;
		




strncpy(sender,fd+sizeof(hdr->op),sizeof(hdr->sender));

sender[strlen(sender)]=0;

if(strlen(sender)==0 || op<REGISTER_OP || op>DISCONNECT_OP)
	return -1;


setHeader(hdr,op,sender);




return N;

}


int DataMsgs( message_data_t *data,int sd){

char *buffer=calloc(1,sizeof(data->hdr.receiver)+sizeof(data->hdr.len)+1);  //buffer che conterrà il l'header di data del messaggio
unsigned int len=0;

//leggo header di data del messaggio
readn(sd,buffer,sizeof(data->hdr.receiver)+sizeof(data->hdr.len));

char *receiver=calloc(1,sizeof(data->hdr.receiver));

char buffer2[sizeof(data->hdr.len)+1];



strncpy(receiver,buffer,sizeof(data->hdr.receiver));

receiver[strlen(receiver)]=0;


strncpy(buffer2,buffer+sizeof(data->hdr.receiver),sizeof(data->hdr.len));

//trasformo il valore char in unsigned int  e lo inserisco su len													
sscanf(buffer2,"%u",&len);
																		

char *buf=calloc(1,len+1);

//leggo il messaggio vero e proprio
readn(sd,buf,len);

buf[len]=0;

setData(data,receiver,buf,len);



free(buffer);
if(!fbackup(receiver))return -1;



if(!fbackup(buf))return -1;

return 0;
}





 int DataFile(int fd, message_data_t *data,char*nome,int MaxFileSize)
 {


int ret=0;            //valore di lettura                         
unsigned int len=0;  


char *buffer=calloc(1,sizeof(data->hdr.receiver)+sizeof(data->hdr.len)+3);   //buffer che conterrà l'header di data del messaggio
char *receiver=calloc(1,sizeof(data->hdr.receiver)+1);
char *buf;                   //buffer che conterrà il contenuto del file
char *bufferlen=calloc(1,sizeof(data->hdr.len)+3);




if(!fbackup(buffer))return -1;
if(!fbackup(receiver))return -1;
if(!fbackup(bufferlen))return -1;



if(!fd)
{
  errno=EINVAL;
     return -1;
}


 if (( ret= readn( fd , buffer,sizeof(data->hdr.receiver)+sizeof(data->hdr.len)+2)   ) <= 0)   
     {  

        printf("[+]Client disconnected by %d  \n" ,fd);   
             
         return 0 ;
   
      }
                     

else
{

strncpy(receiver,buffer,sizeof(data->hdr.receiver));
receiver[strlen(receiver)]=0;

strncpy(bufferlen,buffer+sizeof(data->hdr.receiver),sizeof(data->hdr.len)+2);

//trasformo il valore char in unsigned int  e lo inserisco su len                         
sscanf(bufferlen,"%u",&len);

  
buf=malloc(len+1);



char*nomefile=calloc(1,strlen(nome)+strlen(DirName)+5); //nome utilizzato per la creazione del file in cui scrivere

strcat(nomefile,DirName);

strcat(nomefile,"/");

char* store = strrchr(nome,'/');


if(store == NULL)
  strcat(nomefile,nome);
else
  strcat(nomefile,store+1);

  



FILE* fPtr;

fPtr=fopen(nomefile,"w");

if(fPtr==NULL){

free(buf);
    puts("[-]fptr null");
    return -1;
}

readn( fd, buf,len);


int w=0;
for(w=0;w<len;w++)
fprintf(fPtr,"%c",buf[w]);

fclose(fPtr);



if(len>=MaxFileSize*1000)
        return OP_MSG_TOOLONG ;
    



free(nomefile);
free(buf);




}


return OP_OK;
}



void libera_spazzatura(){


for(int j=0;j<MAX_BACKUP;j++)

  if(backup[j]!=NULL)
  
    free(backup[j]);
    


}