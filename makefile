compile: allocate.c
	gcc -pedantic -Wall -g -c src/allocate.c

test: tests.c allocate.c
	gcc -pedantic -Wall -g src/tests.c src/allocate.c -o src/test -lcunit

clean:
	rm compile
