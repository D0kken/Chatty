
/**
 *   Stefano Russo 544341
 *
 *   stefanoruss97@gmail.com   
 *
 *  Si dichiara che il contenuto di questo file e' in ogni sua parte opera
 *    originale dell'autore
 */


#include<stdlib.h>

#include<extern.h>
#include<connections.h>



int chint2(char* string)
{   

char buf[10];
int numero=0;


strncpy(buf,string,10);




switch(strlen(string)){

  case 0: return -1;
  break;

  case 1: numero=  ((int)string[0] - 48);
  break;
 
  case 2: numero=  (10*((int)string[0] - 48)) + ((int)string[1] - 48) ;
  break;

   case 3:numero=  (100*((int)string[0] - 48)) + (10*((int)string[1] - 48))  + ((int)string[2] - 48);
  break;
 
  case 4: numero=  (1000*((int)string[0] - 48))+ (100*((int)string[1] - 48))+ (10*((int)string[2] - 48))+ ((int)string[3] - 48);
  
  break;
}



return numero;
}



int parsing_config(char* nomefile)
{
  FILE *fp;
  char *str=malloc(100);


  fp = fopen(nomefile, "r");
  if(fp == NULL) 
  {
     perror("Error opening file");
     return -1;
  }

 while( fgets (str, 100, fp)!= NULL ) 
 {
    char *buffer1 = calloc(1,100);
    char *buffer2 = calloc(1,100);

    if(str[0] != '#')
    {
      if(sscanf(str,"%s\t = %s",buffer1,buffer2) != EOF)
       {

        

           if(strcmp(buffer1,"UnixPath")==0)
            {
              UnixPath=calloc(1,1+strlen(buffer2));
              if(!fbackup(UnixPath))return -1;
             strncpy(UnixPath,buffer2,strlen(buffer2));
             }
 
           else if(strcmp(buffer1,"MaxConnections")==0)
                 MaxConnections=chint2(buffer2);
          

           else if(strcmp(buffer1,"ThreadsInPool")==0)
             ThreadsInPool=chint2(buffer2);
         

           else if(strcmp(buffer1,"MaxMsgSize")==0)
            MaxMsgSize=chint2(buffer2);
         

           else if(strcmp(buffer1,"MaxFileSize")==0)
            MaxFileSize=chint2(buffer2);
          

           else if(strcmp(buffer1,"MaxHistMsgs")==0)
            MaxHistMsgs=chint2(buffer2);
      

           else if(strcmp(buffer1,"StatFileName")==0)
           {  StatFileName=calloc(1,1+strlen(buffer2));
            if(!fbackup(StatFileName))return -1;
             strncpy(StatFileName,buffer2,strlen(buffer2));
           }

           else if(strcmp(buffer1, "DirName")==0)
           {
             DirName=calloc(1,strlen(buffer2)+1);
             if(!fbackup(DirName))return -1;
             strncpy(DirName,buffer2,strlen(buffer2));
           }
     

          } 

     }
free(buffer1);
free(buffer2);

 }

fclose(fp);


free(str);

if( UnixPath      ==NULL ||
    StatFileName  ==NULL ||
    DirName       ==NULL ||
    MaxConnections==0    ||
    ThreadsInPool ==0    ||
    MaxMsgSize    ==0    ||
    MaxFileSize   ==0    ||
    MaxHistMsgs   ==0    
  )

	return -1;



  
  return 0;
}




