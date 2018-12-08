#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char** capturar(){
	char *comando;
	char **tokens = NULL;
	int i = 0;
	comando = (char *) malloc(sizeof(char));
	fgets(comando,140,stdin);
	if(*comando != '\n'){
		tokens = (char **) malloc(sizeof(char *));
		char *token;
		for(token = strtok(comando," ") ; token != NULL ; token = strtok(NULL," "), i++){
			tokens = (char **) realloc(tokens,sizeof(char*) * (i + 1));
			*(tokens + i) = (char *) malloc(1 + strlen(token));
			strcpy(*(tokens + i),token);
		}
		free(token);
	}
	fflush(stdin);
	free(comando);
	return tokens;
}

int ptrlen(char **ptr){
	printf("len\n");
	int i;
	for(i = 0 ; *(ptr + i) != NULL && **(ptr + i) != '\n' ; i++);
	return i;
}

void ptrfree(char **ptr){
	printf("free\n");
	int len = ptrlen(ptr);
	for(int i = 0 ; i < len ; i++) {
		free(*(ptr + i));
	}
	free(ptr);
}

int main(){
	char **tokens;
	while(1){
		printf("MiniShell>");
		tokens = capturar();
		printf("Salio\n");
		if(tokens != NULL){
			printf("for\n");
			int len = ptrlen(tokens);
			for(int i = 0 ; i < len ; i++) {
				printf("%d:%s\n",i,*(tokens + i));
			}
			ptrfree(tokens);
		}
	}
}