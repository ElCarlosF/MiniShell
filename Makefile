src = minishell.c
bin = minishell

compilar:
	gcc $(src) -o $(bin)

minishell:
	gcc minishell.c

clear:
	rm *.o
	rm $(bin)