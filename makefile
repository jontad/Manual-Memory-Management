compile: src/allocate.c
	gcc -pedantic -Wall -g -c src/allocate.c

test: src/tests.c src/allocate.c
	gcc -pedantic -Wall -g src/tests.c src/allocate.c -o src/test -lcunit
	./src/test
clean:
	rm compile
