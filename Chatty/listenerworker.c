/**
 *   Stefano Russo 544341
 *
 *   stefanoruss97@gmail.com   
 *
 *  Si dichiara che il contenuto di questo file e' in ogni sua parte opera
 *    originale dell'autore
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include<signal.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros  
#include <unistd.h>//read write

#include<listenerworker.h>
#include<connections.h>


void Gestione(void* sd, int localcurr)
{
  
  assert(sd);                            
  
 
  int ret=0;                         //variabile che salva il valore di ritorno delle funzioni chiamate                

 
  int fd=(intptr_t)sd;              //file descriptor del client di cui si sta eseguendo la richiesta           

 
  char * connessi=calloc(1,MAX_NAME_LENGTH*MaxConnections);    //buffer contenete i nickname dei connesi     

  fbackup(connessi);

  int valread=0;
  int readsize=sizeof(MSGS[localcurr].hdr.sender)+sizeof(MSGS[localcurr].hdr.op);

char*buffer=calloc(1,readsize+1);
fbackup(buffer);



while(1)
{
	char*nomefile;
 			if ((valread = read( fd,buffer,readsize) == 0) ) 
              { 
                
       

if(pthread_mutex_lock(&connlock)!=0)
    {
        printf("connlock mutex errore");
        return ;
    }

                for(int w=0;w<MaxConnections;w++)
                { 
                  if(conn->file_des[w]==fd)
                  {
                    printf("[+]%s disconnepted from fd: %d\n",conn->Conness[w],conn->file_des[w]);

                    conn->Conness[w]=NULL;
                    conn->file_des[w]=0;
                    conn->nconn--;

                  }
                }

if(pthread_mutex_unlock(&connlock)!=0)
    {
        printf("connlock mutex errore");
        return ;
    }

                close( fd ); 
                break;
                              }//if valread 




	            else
              {//else ha letto

            int ret=1;
                        
            ret=HeaderMsgs(buffer,&MSGS[localcurr].hdr);
                 
        if(ret==-1)
          {
        
  
            setHeader(&MSGS[localcurr].hdr,OP_FAIL,MSGS[localcurr].hdr.sender);

            sendHeader(fd,&MSGS[localcurr].hdr);

            if(statIncr(&chattyStats.nerrors,1)==-1)
          
                   puts("errore nerrors");
        }     

        else
          {
          	
           switch(MSGS[localcurr].hdr.op)
           {
     
           case POSTTXT_OP:
              DataMsgs(&MSGS[localcurr].data,fd);
           break;


           case POSTTXTALL_OP:
              DataMsgs(&MSGS[localcurr].data,fd);
           break;

       
           case POSTFILE_OP:
            
             DataMsgs(&MSGS[localcurr].data,fd);
            
             puts("[+]Downloading file..");
              
          nomefile=calloc(1,strlen(MSGS[localcurr].data.buf));
          fbackup(nomefile);
          nomefile=MSGS[localcurr].data.buf;

            ret=DataFile( fd,&MSGS[localcurr].data,MSGS[localcurr].data.buf,MaxFileSize);
               
                if ( ret <=REGISTER_OP || ret>=OP_FAIL ) 
                   {
                   	
                   setHeader(&MSGS[localcurr].hdr,ret,MSGS[localcurr].hdr.sender);
                   
                   sendHeader(fd,&MSGS[localcurr].hdr);
                      
                   statIncr(&chattyStats.nerrors,1);
                  
                    }

             
            

           break;

           case GETFILE_OP:
           DataMsgs(&MSGS[localcurr].data,fd);

           break;
            
           
           case REGISTER_OP:if(read(fd,buffer,readsize)<=0)
           			perror("listener read :");
           break;
           

           case CONNECT_OP:if(read(fd,buffer,readsize)<=0)
           			perror("listener read :");
           break;
           
           case GETPREVMSGS_OP:if(read(fd,buffer,readsize)<=0)
           			perror("listener read :");
           break;
           

           case USRLIST_OP:if(read(fd,buffer,readsize)<=0)
           		   perror("listener read :");
           break;
           

           case UNREGISTER_OP:if(read(fd,buffer,readsize)<=0)
           			perror("listener read :");
           break;
           

           case DISCONNECT_OP:if(read(fd,buffer,readsize)<=0)
           			perror("listener read :");
           break;

             case CREATEGROUP_OP:
             case ADDGROUP_OP:
             case DELGROUP_OP:
             case OP_OK:
             case TXT_MESSAGE:
             case FILE_MESSAGE:
             case OP_FAIL:
             case OP_NICK_ALREADY:
             case OP_NICK_UNKNOWN:
             case OP_MSG_TOOLONG:
             case OP_NO_SUCH_FILE:
             case OP_NICK_CONNECTED:
             case FREE_BLOCK:
             case OP_END:
             break;


           }
          }
        } 
 
  char*nick=calloc(1,strlen(MSGS[localcurr].hdr.sender)+1);          //nick del client di cui si sta eseguendo la richiesta
  fbackup(nick);
  strcpy(nick,MSGS[localcurr].hdr.sender);
  
  assert(nick);                            
  
  

if(IsConnesso(nick))                            
{

 
 switch ( MSGS[localcurr].hdr.op )                     
  {


  case REGISTER_OP:

//non può registrarsi se è già connesso
     
    setHeader(&MSGS[localcurr].hdr,OP_NICK_ALREADY,nick);

    sendHeader(fd, &MSGS[localcurr].hdr);

    if(ret==-1 || ret>=OP_FAIL)
      if(statIncr(&chattyStats.nerrors,1)==-1)
              puts("errore nerrors"); 
    
    

  break;//case 0
  
  



  case CONNECT_OP:

//non può riconnettersi se è già connesso
    setHeader(&MSGS[localcurr].hdr,OP_NICK_CONNECTED,nick);
    sendHeader(fd, &MSGS[localcurr].hdr);


    if(ret==-1 || ret>=OP_FAIL)
      if(statIncr(&chattyStats.nerrors,1)==-1)
              puts("errore nerrors"); 
    

  break;//case 1







  case POSTTXT_OP:


    
  //non può inviarsi un messaggio da solo
    if(strcmp(MSGS[localcurr].data.hdr.receiver,nick)==0)  
        ret=OP_FAIL;
  

     if(ret!=OP_FAIL) 
       ret=OP_Posttxt( localcurr );

      setHeader(&MSGS[localcurr].hdr,ret,nick);
      sendHeader(fd, &MSGS[localcurr].hdr);

    if(ret==-1 || ret>=OP_FAIL)
      if(statIncr(&chattyStats.nerrors,1)==-1)
              puts("errore nerrors");
    
    
  

    break;//case 2





  case POSTTXTALL_OP:

    ret=OP_Posttxtall( localcurr );

    setHeader(&MSGS[localcurr].hdr,ret,nick);
    sendHeader(fd, &MSGS[localcurr].hdr);

    if(ret==-1 || ret>=OP_FAIL)
      if(statIncr(&chattyStats.nerrors,1)==-1)
              puts("errore nerrors");
    
    

  break;//case 3






  case POSTFILE_OP:


      ret=OP_postfile( fd, localcurr ,nomefile );

    setHeader(&MSGS[localcurr].hdr,ret,nick);

    sendHeader(fd, &MSGS[localcurr].hdr);


   if(ret==-1 || ret>=OP_FAIL)
   	if(statIncr(&chattyStats.nerrors,1)==-1)
            puts("errore nerrors"); 
    

    break;


  case GETFILE_OP://case 4
    
    ret=OP_GetFile( fd, localcurr );

      
    if(ret==-1 || ret>=OP_FAIL)
    {
      setHeader(&MSGS[localcurr].hdr,ret,nick);
      sendHeader(fd, &MSGS[localcurr].hdr);


      if(statIncr(&chattyStats.nerrors,1)==-1)
              puts("errore nerrors");
        
    }
    
  break;//case 5





  case GETPREVMSGS_OP:  
    ret=OP_Prevmsgs(nick,localcurr,fd );

   

  if(ret==-1 || ret>=OP_FAIL)
      
      if(statIncr(&chattyStats.nerrors,1)==-1)
              puts("errore nerrors"); 
    
    


  break;//case 6








  case USRLIST_OP:
 
    
    ret=OP_OK;

    connessi=return_connessi(localcurr);
  
    if (strlen(connessi)==0)
         ret=OP_FAIL;

    setHeader(&MSGS[localcurr].hdr,ret,MSGS[localcurr].hdr.sender);

    setData(&MSGS[localcurr].data,MSGS[localcurr].data.hdr.receiver,connessi,conn->nconn*(MAX_NAME_LENGTH+1));

    sendRequest(fd,&MSGS[localcurr]);

  if(ret==-1 || ret>=OP_FAIL)
    
    	
      if(statIncr(&chattyStats.nerrors,1)==-1)
              puts("errore nerrors"); 
    

  break;//case 7
  





  case UNREGISTER_OP:

    ret=hash_delete((char*)nick);
        
      
    if(ret==-1 || ret>=OP_FAIL)
    {
      setHeader(&MSGS[localcurr].hdr,ret,nick);
      sendHeader(fd, &MSGS[localcurr].hdr);


      if(statIncr(&chattyStats.nerrors,1)==-1)
              puts("errore nerrors");
    }


  
        if(IsConnesso(nick))
          {

            OP_disconnetti(nick,localcurr);



              if(ret==-1 || ret>=OP_FAIL)
              {
                setHeader(&MSGS[localcurr].hdr,ret,nick);
                sendHeader(fd, &MSGS[localcurr].hdr);


                if(statIncr(&chattyStats.nerrors,1)==-1)
                        puts("errore nerrors");
               
                    break;
                }
                  

          }


    mhash_delete( nick);  

    setHeader(&MSGS[localcurr].hdr,ret,nick);
    sendHeader(fd, &MSGS[localcurr].hdr);

    
    if(ret==-1 || ret>=OP_FAIL)
      if(statIncr(&chattyStats.nerrors,1)==-1)
             puts("errore nerrors"); 
  


  break;//case 8






  case DISCONNECT_OP:
      ret=OP_disconnetti(MSGS[localcurr].hdr.sender,localcurr);

      setHeader(&MSGS[localcurr].hdr,ret,nick);
      sendHeader(fd, &MSGS[localcurr].hdr);


      if(ret==-1 || ret>=OP_FAIL)
        if(statIncr(&chattyStats.nerrors,1)==-1)
                  puts("errore nerrors");
      
    


  break;//case 9


  case CREATEGROUP_OP:
  case ADDGROUP_OP:
  case DELGROUP_OP:
  case OP_OK:
  case TXT_MESSAGE:
  case FILE_MESSAGE:
  case OP_FAIL:
  case OP_NICK_ALREADY:
  case OP_NICK_UNKNOWN:
  case OP_MSG_TOOLONG:
  case OP_NO_SUCH_FILE:
  case OP_NICK_CONNECTED:
  case FREE_BLOCK:
  case OP_END:
  break;

  }


}

else 
{

  if(MSGS[localcurr].hdr.op==0)
  {

    ret= OP_Register ( nick ,fd,localcurr);
    
    setHeader(&MSGS[localcurr].hdr,ret,nick);

    if(ret==-1 || ret>=OP_FAIL)
    {
      sendHeader(fd, &MSGS[localcurr].hdr);

      if(statIncr(&chattyStats.nerrors,1)==-1)
              puts("errore nerrors");
     }



    else if(ret==OP_OK)
    {
      //se è stato registrato,ottiene la lista connessi
       connessi=return_connessi(localcurr);
 

       if (strlen(connessi)==0)
         ret=OP_FAIL;


    setData(&MSGS[localcurr].data,MSGS[localcurr].data.hdr.receiver,connessi,conn->nconn*(MAX_NAME_LENGTH+1));

  
    sendRequest(fd,&MSGS[localcurr]);


    }


   
    
  }



  else if(MSGS[localcurr].hdr.op==CONNECT_OP)
  {

    ret=OP_Connect ( nick ,fd ,localcurr );


    setHeader(&MSGS[localcurr].hdr,ret,nick);


     if(ret==-1 || ret>=OP_FAIL)
     {
     
      if(statIncr(&chattyStats.nerrors,1)==-1)
              puts("errore nerrors"); 

       sendHeader(fd, &MSGS[localcurr].hdr);
     }



    else if(ret==OP_OK)
    {
    //se è stato connesso ottiene la lista connessi
       connessi=return_connessi(localcurr);

    if (strlen(connessi)==0)
         ret=OP_FAIL;


    setData(&MSGS[localcurr].data,MSGS[localcurr].data.hdr.receiver,connessi,conn->nconn*(MAX_NAME_LENGTH+1));

    sendRequest(fd,&MSGS[localcurr]);

    }


  }





  else if(MSGS[localcurr].hdr.op == POSTTXT_OP     ||
          MSGS[localcurr].hdr.op == POSTTXTALL_OP  ||
          MSGS[localcurr].hdr.op == POSTFILE_OP    || 
          MSGS[localcurr].hdr.op == GETFILE_OP     || 
          MSGS[localcurr].hdr.op == GETPREVMSGS_OP || 
          MSGS[localcurr].hdr.op == USRLIST_OP     ||
          MSGS[localcurr].hdr.op == UNREGISTER_OP  || 
          MSGS[localcurr].hdr.op == DISCONNECT_OP)
  {

        setHeader(&MSGS[localcurr].hdr,OP_FAIL,nick);

        sendHeader(fd, &MSGS[localcurr].hdr);

        if(statIncr(&chattyStats.nerrors,1)==-1)
                 puts("errore nerrors");
                
    }  


}



}


 MSGS[localcurr].hdr.op=FREE_BLOCK;
 MSGS[localcurr].data.buf=NULL;


}







void *th_Listener() 
{

int w=0;  //indice usato nei for

  
int addrlen=0 , new_socket=0 , activity=0,fd=0;
int max_sd=0;   

fd_set rdset; //insieme fd attesi in lettura  
fd_set set; //insieme file descriptor attivi
       
addrlen = sizeof(sa);   

memset(&sa,0,addrlen);
    

   if(master_socket>max_sd)max_sd=master_socket;
  
    FD_ZERO(&set);   
      
    FD_SET(master_socket , &set);   
      
  //quando è arrivato un segnale di terminazione si arresta   
while(!terminazione|| terminazione==SIGUSR1)   
{  

	int ret=1;

   rdset=set;  

   activity = select( max_sd + 1 , &rdset , NULL , NULL ,NULL);   


    if ((activity < 0) && (errno!=EINTR))   
       {   
            printf("[-]select error");   
     
       }   


else
{			
 

for(fd=0;fd<=max_sd;fd++)
{			

	if(FD_ISSET(fd,&rdset))
	{			
		


		if(fd==master_socket)	//connessione pronta
		{
		
			new_socket=accept(master_socket,NULL,0);
			FD_SET(new_socket,&set);
			
      if(new_socket>max_sd)max_sd=new_socket;
		    	printf("new connection accepted %d\n",new_socket);
		}

		


		else				//sock i/o pronto
		{
		
                 if( ( ret=threadpool_add( (void*)Gestione, (void *) (intptr_t) fd, msgcur )) <0 )
                  {       
                    printf("[-]th_add errore %d \n",ret);
                  
                  }
                  
                  FD_CLR(fd,&set);      
             
                msgcur++;
                msglen++;              
              
  //cerco il primo posto libero su MSGS ovvero quello che ha op FREE_BLOCK
                while(MSGS[msgcur].hdr.op!=FREE_BLOCK && msglen<msgmax)
                  {
                                  
                 msgcur++;                
                 if(msgcur>=msgmax-1)
                        msgcur=0;                   
                  }


        
                

                if(msglen==msgmax)
                {
                  MSGS=realloc(MSGS, (msglen+msgbatch) * sizeof(message_t) ); 
                  msgmax+=msgbatch;
                
  
                if (MSGS == NULL) 
                  {
                   perror("realloc");
                   fprintf(stderr, "ERRORE: Out of memory\n");
                   exit(EXIT_FAILURE);
                  }

          //inizializzo i nuovi elementi
        		  for(w=msglen;w<msgmax;++w) 
          			{
          				MSGS[w].data.buf=NULL; 
            			MSGS[w].hdr.op=FREE_BLOCK;    
          			} 
          
         	      } // if len=max

                




             } 


              } 
		
		       
	        } 
     
     } 
   
}

return(NULL);
      
}  
