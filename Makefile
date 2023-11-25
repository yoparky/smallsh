smallsh: main.o commands.o utilities.o
	gcc -std=gnu99 -o smallsh main.o commands.o utilities.o

main.o: main.c commands.h utilities.h data_struct.h
	gcc -std=gnu99 -c main.c

commands.o: commands.c commands.h data_struct.h
	gcc -std=gnu99 -c commands.c

utilities.o: utilities.c utilities.h data_struct.h
	gcc -std=gnu99 -c utilities.c

clean:
	rm -f smallsh *.o
