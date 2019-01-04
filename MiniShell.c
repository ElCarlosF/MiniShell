/* Programa [JASHELL.c]
   Versión  [5.57]
   Autores  [Jose Alberto Benítez Andrades \/ infjab02@estudiantes.unileon.es]
	    [Alejandro Fernández Aparicio \/ infafa02@estudiantes.unileon.es]
   Fecha    [04/05/2007 - 15/05/2007]
   * 
   * Modificado por Carlos Faundez Muñoz
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include "g.h"

#define compara " "
#define MSJ_ERROR "Error de sintaxis\n"
#define MAX 51
#define PROMPT "@MiniShell"
#define EXIT "quit\n\0"
size_t len;
char directorio[1024];

int Cadexit(char *cadena);
void Comando(char cadena[MAX], int plano);
void redirecEntrada(char cadena[MAX]);
void redirecSalida(char cadena[MAX]);
void tuberias(char* argumento1[MAX], char* argumento2[MAX]);
void CrearProceso(char* argumento[MAX], int plano);
char* verif_syntax(char *string);
int comandoespeciales(char *string);


/* La función principal main contiene el bucle que se ejecutará de forma contínua hasta que
el usuario escriba FIN en el terminal.Recibe una cadena, la analiza, y a partir de las 
funciones que hemos creado, hará lo que debe de hacer */

int main() { /* INICIO DE LA FUNCIÓN main */
	//char cadena[MAX];
	char *user;
	char *cadena;
	char *comando = '\n';
	user = getenv("USER");
	getcwd(directorio, sizeof(directorio));
	//char cadFin[]="exit";
	int i = -1, segplano = 0, guardaStdout = dup(1), guardaStdin = dup(0);
	system("clear");
	while(1) {
		close(1); /* Cierro la salida que tenga, caso de haber guardado algo en un fichero
					será el fichero */
		dup(guardaStdout); // Asigno la salida estándar, es decir, la consola.
		close(0); /* Cierro la entrada que tenga */
		dup(guardaStdin); // Asigno la entrada estándar, es decir, el teclado.
		/*printf(PROMPT); // Imprimimos el prompt
		scanf("\n%[^\n]",cadena); // Escaneamos la cadena entera hasta que pulsa intro */
		
		/*do{
			if(!i){
				if(*cadena != '\n'){
					printf("?SYNTAX  ERROR\n");
				}
			}
			printf(PROMPT);
			getline(&cadena,&MAX,stdin);
		}while(!(i = verif_syntax(cadena)));*/
		
		do{
			if(comando == NULL){
				if(*cadena != '\n'){
					printf("?SYNTAX  ERROR\n");
				}
			}
			printf("[%s%s %s]$ ",user,PROMPT,directorio);
			getline(&cadena,&len,stdin);
		}while((comando = verif_syntax(cadena)) == NULL);
		
		segplano = 0;
		for(i = 0; *(comando + i) != '\n'; i++) {
			if(*(comando + i) == '&') {
				*(comando + i) = '\0';
				segplano = 1;
			}
		}
		//CadIguales(cadena,cadFin); // Comparamos si la cadena es fin, en tal caso sale
		if(Cadexit(comando)){
			printf("BYE! =3\n");
			exit(0);
		}
		Comando(comando,segplano); // Y si no es fin, entra en la función Comando
		//printf("Free\n");
		//free(cadena);
		//free(comando);
		//printf("End free\n");
	}
	return 0;
} /* FIN DE LA FUNCIÓN main */

/* Con esta función compararemos la cadena que introduce el usuario, con FIN, 
en caso de ser iguales, saldrá del programa, de lo contrario devolverá 0  y el programa
seguirá en ejecución */

int Cadexit(char *cadena) { // inicio del Cadexit
	/*int i=0;
	int igual=0;
	while ((cadena1[i]==cadena2[i])&&(cadena1[i]!='\0')&&(cadena2[i]!='\0')) i++;	
	if ((cadena1[i]==cadena2[i])&&(cadena1[i]=='\0')&&(cadena2[i]=='\0')) exit(0);				
	return(igual);*/
	
	if(strcmp(cadena,EXIT) == 0) {
		return 1;
	}
	return 0;
		
	
}//fin Cadexit

void Comando(char *cadena, int plano) { /* INICIO DE LA FUNCIÓN COMANDO */
	int i = 0, j, k = 0, flag_tuberia = 0, ejecutar = 0;
	char argumentoInd[MAX][MAX];
	char argumentoInd2[MAX][MAX];
	char redirec[MAX];
	char entrada[MAX];
	char *argumento[MAX];
	char *argumento2[MAX];
	argumento[0] = NULL;					
	argumento2[0] = NULL;
	//while(cadena[i] != '\0' && cadena[i] != '|' && cadena[i] != '>') { /* INICIO DE WHILE */	
	while(*(cadena + i) != '\n' && *(cadena + i) != '|' && *(cadena + i) != '>') { /* INICIO DE WHILE */	
		/* Con este for recorremos la cadena por completo, y la vamos
		almacenando en una variable que consiste en un array bidimensional de tipo char, 
		es decir almacena cadenas de cadenas de caracteres */
		//for(j = 0; cadena[i] != ' ' && cadena[i] != '\0' && cadena[i] != '|'  && cadena[i] != '>' && cadena[i] != '<';j++) {
		for(j = 0; *(cadena + i) != ' ' && *(cadena + i) != '\n' && *(cadena + i) != '|'  && *(cadena + i) != '>' && *(cadena + i) != '<';j++) { 
			argumentoInd[k][j] = cadena[i++];
			//i++;
		}

		/* Comprobamos si la cadena salió del bucle por encontrar un espacio...
		en tal caso se incrementa la i, ya que sino entraría en un bucle 
		infinito*/
		//if (cadena[i] == ' ') i++;
		if (*(cadena + i) == ' ') i++;

		/* Asignamos el terminador de cadena a cada comando que es leído */
		argumentoInd[k][j] = '\0';

		/* Y finalmente una vez creada la cadena, se la pasamos al puntero 
		argumento que será el que se ejecute con la función execvp */
		argumento[k] = argumentoInd[k];
		k++;
		//if (cadena[i] == '<') { /* INICIO DEL IF */
		if (*(cadena + i) == '<') { /* INICIO DEL IF */
			i++;
			//if (cadena[i] != ' ') ejecutar = 1;
			if (*(cadena + i) != ' ') ejecutar = 1;
			else { 
				i++;
				//for(j = 0; cadena[i] != '\0' && cadena[i] != ' ' && cadena [i] != '|' && cadena [i] != '>'; j++){
				for(j = 0; *(cadena + i) != '\n' && *(cadena + i) != ' ' && *(cadena + i) != '|' && *(cadena + i) != '>'; j++){
					entrada[j] = cadena[i++];
					//i++;
				}
				entrada[j] = '\0';
				//if (cadena[i] != '\0') i++;
				if (*(cadena + i) != '\n') i++;
					redirecEntrada(entrada);
				}
		}/* FIN DEL IF */
					
	} /* FIN DEL WHILE */

	argumento[k] = NULL; // Asignamos NULL al último comando a ejecutar para el EXECV
	    
	/* Si encuentra una tubería, entrará en el IF y separará el segundo comando de la 
	misma forma que se hizo con el primero, y finalmente llamará a la función tuberias,
	pasándole los 2 argumentos como parámetros */

	//if (cadena[i] == '|') {/* INICIO DE IF */
	if (*(cadena + i) == '|') {/* INICIO DE IF */
		k=0;
		i++;
		//if (cadena[i] != ' ') ejecutar=1;
		if (*(cadena + i) != ' ') ejecutar = 1;
		else { /* INICIO DEL ELSE */
			i++;
			flag_tuberia = 1;
			//while(cadena[i] != '\0' && cadena[i] != '>'){/* INICIO DE WHILE */	 
			while(*(cadena + i) != '\n' && *(cadena + i) != '>'){/* INICIO DE WHILE */	 
					//for(j=0 ; cadena[i] != ' ' && cadena[i] != '\0' && cadena[i] != '>' ; j++) {
					for(j = 0 ; *(cadena + i) != ' ' && *(cadena + i) != '\n' && *(cadena + i) != '>' ; j++) {
								argumentoInd2[k][j] = cadena[i++];
								//i++;
					}
					//if (cadena[i] == ' ' ) i++;
					if (*(cadena + i) == ' ' ) i++;
			
					argumentoInd2[k][j] = '\0';
					argumento2[k] = argumentoInd2[k];
					k++;
				}/* FIN DEL WHILE */
			argumento2[k] = NULL;
			}/*FIN EL ELSE */		
		}/* FIN DE IF */


		/* Si encuentra un > cortará la cadena que será el fichero que quiere utilizar
		para la salida */
		//
		//if (cadena[i] == '>') {/* INICIO DE IF */
		if (*(cadena + i) == '>') {/* INICIO DE IF */
			i++;
			//if (cadena[i] != ' ') ejecutar = 1;
			if (*(cadena + i) != ' ') ejecutar = 1;
			else {
				i++;
				//for(j = 0 ; cadena[i] != '\0' ; j++) {
				for(j = 0 ; *(cadena + i) != '\n' ; j++) {
					redirec[j] = *(cadena + i);
					i++;
				}
				redirec[j] = '\0';
				redirecSalida(redirec);	
			}
		}/* FIN DE IF */
		
		/* Comprobamos si la variable ejecutar tiene valor 0 o 1, si tiene valor 0
		el programa se ejecutará correctamente, si tiene valor 1 significa que 
		mientras analizaba alguna de las cadenas ha encontrado un error de sintaxis
		y mostrará en pantalla un mensaje de error. */
		
		if(!ejecutar) {
			if (flag_tuberia == 0) CrearProceso(argumento,plano);	
			else tuberias(argumento,argumento2);
		}
		//else printf( MSJ_ERROR );
		
}/* FIN DE LA FUNCIÓN COMANDO */

/* Con esta función si el usuario introduce quiere cambiar la entrada estándar
( el teclado ) por lo que hay en un fichero, podrá hacerlo mediante el símbolo 
"<", por ejemplo: wc < fichero */

void redirecEntrada(char *cadena) { /* INICIO DE LA FUNCIÓN redirecEntrada */
	char *cadenaPtr;
	int fd;  
	cadenaPtr = cadena; // Puntero a la cadena
	fd = open (cadenaPtr,O_RDONLY); // Asigno a la salida el fichero
	close (0); // Cierro la salida estándar	
	dup(fd);
} /* FIN DE LA FUNCIÓN redirecEntrada */

/* Con esta función si el usuario introduce una redirección de salida a un 
fichero, en lugar de mostrar el comando en pantalla, lo guardará en fichero
por ejemplo: man -k file > lista.file */

void redirecSalida(char *cadena) { /* INICIO DE LA FUNCIÓN redirecSalida */
	char *cadenaPtr;
	cadenaPtr = cadena; // Puntero a la cadena
	close (1); // Cierro la salida estándar		
	open (cadenaPtr,O_CREAT | O_WRONLY, 0777); // Asigno a la salida el fichero
} /* FIN DE LA FUNCIÓN redirecSalida */


/* Con esta función en el caso de que haya tuberías ( se ejecuten dos comandos 
concatenados) por ejemplo: ls -la | sort , recibirá como parámetro argumento1 y 
argumento2 de tipo puntero a cadena de cadena de caracteres y creará un hijo que
será el encargado de ejecutar los 2 comandos pasados por tubería.*/

void tuberias(char* argumento1[MAX], char* argumento2[MAX]) { /* INICIO DE LA FUNCIÓN tuberias */
	int fd[2],estado,i;
	pid_t hijo;
	char *echo;
	hijo = fork();
	if (hijo == -1) printf("ERROR Creacion de proceso"); 
	else if (hijo == 0) {
		pipe(&fd[0]); /* Función pipe encargada de crear la tubería */
		if ((hijo = fork()) == 0) {
			close(fd[0]); /* cerramos el lado de lectura del pipe */
			close(1);
			dup(fd[1]); /* STDOUT = extremo de salida del pipe */
			close(fd[1]); /* cerramos el descriptor de fichero que sobra 
						  tras el dup(fd[1])*/
			switch(comandoespeciales(argumento1[0])){
				case 1:
					chdir(*(argumento1 + 1));
					getcwd(directorio,sizeof(directorio));
					exit(0);
				case 2:
					echo = (char *) malloc(sizeof(char)*140);
					strcat(echo,"echo '");
					for(i = 1 ; *(argumento1 + i) != NULL ; i++){
						//sprintf(echo,"%s %s",echo,*(argumento + i));
						strcat(echo,strcat(*(argumento1 + i)," "));
					}
					strcat(echo,"'");
					//sprintf(*argumento,"%s %s ",*argumento,*(argumento + 1));
					//printf("%s\n",echo);
					system(echo);
					free(echo);
					exit(0);
					
			}
			execvp(argumento1[0],argumento1);
		}
		else {
			close(fd[1]);/* cerramos el lado de lectura del pipe */
			close(0);
			dup(fd[0]);/* STDOUT = extremo de salida del pipe */
			close(fd[0]);/* cerramos el descriptor de fichero que sobra 
						tras el dup(fd[0])*/
			switch(comandoespeciales(argumento2[0])){
				case 1:
					chdir(*(argumento2 + 1));
					getcwd(directorio,sizeof(directorio));
					exit(0);
				case 2:
					echo = (char *) malloc(sizeof(char)*140);
					strcat(echo,"echo '");
					for(i = 1 ; *(argumento2 + i) != NULL ; i++) {
						//sprintf(echo,"%s %s",echo,*(argumento + i));
						strcat(echo,strcat(*(argumento2 + i)," "));
					}
					strcat(echo,"'");
					//sprintf(*argumento,"%s %s ",*argumento,*(argumento + 1));
					//printf("%s\n",echo);
					system(echo);
					free(echo);
					exit(0);
			}
			while(waitpid(hijo,&estado,0) != hijo);
			execvp(argumento2[0],argumento2);
		}
	}
	else {
		while(waitpid(hijo,&estado,0) != hijo);
	}//hijo = wait(&estado);
} /* FIN DE LA FUNCIÓN tuberias */


/* Con esta función en el caso de que no haya tuberías y sea un sólo comando a ejecutar,
por ejemplo: ls -la , recibirá como parámetro el argumento de tipo puntero a cadena
de cadena de caracteres, creará un hijo para que el comando execvp no finalice el
programa y en definitiva ejecutará el comando que le pedimos.*/

void CrearProceso(char* argumento[MAX],int plano) {	/* INICIO DE LA FUNCIÓN CrearProceso */
	int i;
	int estado = 0;
	int ctrl = comandoespeciales(argumento[0]);
	pid_t hijo;
	char *echo;
	if(ctrl > 0){
		switch (ctrl){
			case 1:
				switch(chdir(*(argumento + 1))){
					case 0: getcwd(directorio,sizeof(directorio));
					break;
					case EACCES: perror("Permiso denegado\n");
					break;
					case EIO:	 perror("Un error I/O ocurrio\n"); //An input output error occured
					break;
					case ENAMETOOLONG: perror("Ruta con dimension grande\n"); //Path is to long
					break;
					case ENOTDIR: perror("Una parte de la ruta no es diretorio\n"); //"A component of path not a directory
					break;
					case ENOENT: perror("No such file or directory\n"); printf("enoent\n");
					break;
					default: printf("No se pudo cambiar a %s\n", *(argumento + 1));
						
				}
				break;
			case 2:
				echo = (char *) malloc(sizeof(char)*140);
				strcat(echo,"echo '");
				for(i = 1 ; *(argumento + i) != NULL ; i++){
					//sprintf(echo,"%s %s",echo,*(argumento + i));
					strcat(echo,strcat(*(argumento + i)," "));
				}
				strcat(echo,"'");
				//sprintf(*argumento,"%s %s ",*argumento,*(argumento + 1));
				//printf("%s\n",echo);
				system(echo);
				free(echo);
				break;
			case 3:
				printf("***********Ayuda***********\n");
				printf("quit: Sales de la shell\n");
				printf("ayuda: Imprime este mensaje\n");
				break;
			case 4:
				sl();
				break;
		}
	}else{
		hijo = fork();
		/* Comprobamos si el hijo se creó bien */
		if (hijo == -1) printf("ERROR Creacion de proceso"); 
		else if (hijo == 0) {
			/* Y en caso de que el hijo esté bien creado, ejecutamos el proceso,
			si el programa comando a ejecutar no existe, nos da un error.*/
			execvp(argumento[0],argumento);
			perror("MiniShell");
			exit(estado);
		}
		else { 
			if (plano == 0) hijo = wait(&estado);
		}
	}

} /* FIN DE LA FUNCIÓN CrearProceso */

char* verif_syntax(char *string) {
	char *aux = (char *) malloc(sizeof(char) * MAX);
	strcpy(aux,string);
	if(*(string) != '\n' && strstr(string,"[") == NULL && strstr(string,"]") == NULL && strstr(string,"(") == NULL&& strstr(string,")") == NULL
	&& strstr(string,"=") == NULL && strstr(string,"%") == NULL && strstr(string,"\\") == NULL && strstr(string,"¬") == NULL
	&& strstr(string,"<<") == NULL && strstr(string,">>") == NULL && strstr(string,"  ") == NULL && *(string+(strlen(string) - 2)) != ' ' ){		
		for (int i = 0 ; i < strlen(string) - 1 ; i++) {
			if((*(string + i) == '|' || *(string + i) == '>' || *(string + i) == '<')) {
				if(*(string + i - 1) != ' ' && *(string + i + 1) != ' ' && *(string + i + 2) != '\n') {
					free(aux);
					return NULL;
				}
			}else if(*(string + i) == '&' && *(string + i - 1) != ' ' && *(string + i + 2) != '\n'){
				free(aux);
				return NULL;
			}
		}
		return aux;
	}
	free(aux);
	return NULL;
}

int comandoespeciales(char *string) {
	if(strcmp(string,"cd") == 0){
		return 1;
	}
	if(strcmp(string,"echo") == 0){
		return 2;
	}
	if(strcmp(string,"ayuda") == 0){
		return 3;
	}
	if(strcmp(string,"sl") == 0){
		return 4;
	}
	return 0;
}
