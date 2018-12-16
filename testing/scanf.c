#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void alarma();
int i = 0;
char **token;
int main(){
	char *string = (char *)malloc(sizeof(char));
	token = (char **) malloc(sizeof(char *));
	signal(SIGALRM,alarma);
	do{
		token = (char **) realloc(token,sizeof(char *) * (i+1));
		scanf("%s",string);
		ualarm(100000,0);
		*(token + i) = (char *) malloc(sizeof(char)*(1 + strlen(string)));
		strcpy(*(token + i),string);
		++i;
	}while(getchar() != '\n');
	signal(SIGINT,alarma);
	for(int j = 0 ; j < i ; j++){
		printf("|%s|",*(token + j));
	}
	return 0;
}

void alarma(){
	signal(SIGINT,alarma);
	for(int j = 0 ; j < i ; j++){
		printf("|%s|",*(token + j));
	}
	exit(0);
}
