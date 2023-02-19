
all: shell server

shell: myShell.o commands.o
	gcc myShell.o commands.o -o shell

myShell.o: myShell.c commands.h
	gcc -c -g myShell.c

commands.o: commands.c commands.h
	gcc -c -g commands.c

server: server.o
	gcc server.o -o server

server.o: server.c 
	gcc -c server.c 

clean:
	rm -f *.o server shell