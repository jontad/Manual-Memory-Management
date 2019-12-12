compile: src/allocate.c
	gcc -pedantic -Wall -g -c src/allocate.c

tests: test/tests.c src/allocate.c
	gcc -pedantic -Wall -g test/tests.c src/allocate.c -o test/tests -lcunit
	./test/tests

val_tests:
	valgrind --leak-check=full ./test/tests

clean:
	rm ./test/tests
