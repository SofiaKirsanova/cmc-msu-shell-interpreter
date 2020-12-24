all: main list tree exec
	gcc -o prog main.o list.o tree.o exec.o

main: main.c
	gcc -c main.c

list: list.c
	gcc -c list.c

tree: tree.c
	gcc -c tree.c

exec: exec.c
	gcc -c exec.c

clear:
	rm *.o
