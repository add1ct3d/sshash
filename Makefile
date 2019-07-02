sshash:main.o
	gcc -o sshash main.o -lssh -lncurses
main.o:main.c
	gcc -o main.o -c main.c

