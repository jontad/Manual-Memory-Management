compile: allocate.c
	gcc -pedantic -Wall -g -c allocate.c

test: tests.c allocate.c
	gcc -pedantic -Wall -g tests.c allocate.c -o test -lcunit

clean:
	rm compile
