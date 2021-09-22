/**
 *   Stefano Russo 544341
 *
 *	 stefanoruss97@gmail.com   
 *
 *  Si dichiara che il contenuto di questo file e' in ogni sua parte opera
 *    originale dell'autore
 */


#ifndef PARSCONFIG_H_
#define PARSCONFIG_H_



/**
 * @function chint2
 * @brief trasforma un numero char in un int per un massimo di 4 cifre
 *
 * @param string stringa di numero
 *
 * @return 0 in caso di successo, -1 in caso di fallimento 
 */

int chint2(char* string);

/**
 * @function parsing_config
 * @brief Esegue il parsing del file di configurazione passato
 *
 * @param nomefile file di configurazione da leggere
 *
 * @return			 0 in caso di successo
 					 -1 in caso di fallimento 
 */
//prende in input una stringa as pathname del file di configurazione,e legge nelle variabili globali,i loro rispettivi valori contenuti nel file
int parsing_config(char* nomefile);


#endif /* PARSCONFIG_H_ */


