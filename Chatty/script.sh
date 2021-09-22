 #
 #   Stefano Russo 544341
 #
 #   stefanoruss97@gmail.com   
 #
 #  Si dichiara che il contenuto di questo file e' in ogni sua parte opera
 #    originale dell'autore
 #




#!/bin/bash


#se viene usato il numero errato di argomenti o si utilizza il comando --help stampa in output il messaggio di uso
if [ $# != 2 ] || [ "$1" == "--help" ] || [ "$2" == "--help" ]; then			
    echo "Uso: [NOME SCRIPT] [PATHNAME FILE CONFIGURAZIONE] [INTERO POSITIVO]"
    exit 1
fi


#se il secondo argomento non è un intero positivo ritorna messaggio di errore
if ! [[ "$2" =~ ^[0-9]+$ ]]; 
    then
        echo "Wrong error"
        exit 1
fi


Config=$1
t=$2



#estraggo il valore di DirName contenuto sul file di configurazione passato per argomento (Config)
r=$(head -40 $Config | grep "DirName" | cut -d\, -f 3 | cut -d"=" -f 2 | cut -d"/" -f 1,2,3 | tail -1)


if ! [[ $r != 0 ]]; then
    echo “Errore”
    exit 1
fi



#mi sposto nel path indicato da DirName 
cd $r



#se t=0 stampo in output la lista dei file 
if [ $t -eq 0 ]; then
	echo "$(ls -1)"
	exit 1
fi	



#creo la lista dei file più vecchi di t minuti
   for i in $( ls -1); do
            echo item: $i
            d=$(($(date +%-M)-$(date +%-M -r $i ) ))
        	echo $d

        	if [[ "$d" -gt "$t" ]]; then
        		lista+="$i "
        	fi
        		
        done
        


#archivio i file più vecchi di t minuti 
if ! [[ -z "$lista" ]]; then
	#echo $lista
	#echo "gg"
	r=$(tar -cvf go.tar.gz $lista)


#se l'archiviazione è andata a buon fine
  if ! [[ -z "$r" ]]; then  
	rm $r
  fi



fi



