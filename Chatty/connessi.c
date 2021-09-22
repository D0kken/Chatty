/**
 *   Stefano Russo 544341
 *
 *	 stefanoruss97@gmail.com   
 *
 *  Si dichiara che il contenuto di questo file e' in ogni sua parte opera
 *    originale dell'autore
 */

#include<stdlib.h>

#include<config.h>
#include<extern.h>
#include<connections.h>



int IsConnesso(char*nick)
{

int k=0;

for(k=0;k<MaxConnections;k++)

	if(conn->Conness[k]!=NULL)

		if(strcmp(conn->Conness[k],nick)==0)
			return 1;

return 0;
}





char* return_connessi(int localcurr)
{


char* string=calloc(1,(MAX_NAME_LENGTH+1) * MaxConnections);
fbackup(string);
int k=0;
int i=0;

if(pthread_mutex_lock(&connlock)!=0)
					    {
 	 					printf("connlock mutex errore");
	   						 return NULL;
				     	} 
  



for(i=0;i<MaxConnections;i++)
 {

   if(conn->Conness[i]!=NULL && conn->file_des[i]>0)
   	 {
 		strncpy(string+k,conn->Conness[i],MAX_NAME_LENGTH);
      	k+=	MAX_NAME_LENGTH+1;
 
 	
 	  }

 }

if(pthread_mutex_unlock(&connlock)!=0)
					    {
 	 					printf("connlock mutex errore");
	   						 return NULL;
				     	} 

string[k]=0;


return string;
}





Connessi* connessi_create(){

Connessi* con=calloc(1,sizeof(Connessi));
memset(con,0,sizeof(Connessi));

if(con==NULL)
	return NULL;


return con;
}
