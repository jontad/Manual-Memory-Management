C_COMPILER     = gcc
C_OPTIONS      = -Wall -ggdb -pedantic -g
C_LINK_OPTIONS = -lm 
CUNIT_LINK     = -lcunit
C_LCOV 	       = --coverage 	



compile: src/allocate.c src/cascade.c test/lib_for_tests.c
	$(C_COMPILER) $(C_OPTIONS) -c $^

test_compile: test/tests.c src/allocate.c src/cascade.c test/lib_for_tests.c
	$(C_COMPILER) $(C_LCOV) $(C_OPTIONS) $^ -o test/tests $(CUNIT_LINK)


tests: test_compile
	./test/tests	

val_tests: test_compile
	valgrind --leak-check=full ./test/tests

clean:	
	rm ./test/tests

lcov_generate: tests
	lcov --capture --rc lcov_branch_coverage=1 --directory . --output-file test/tests.info

lcov: lcov_generate
	genhtml --rc genhtml_branch_coverage=1 test/tests.info -o test/tests-lcov

lcov_open: lcov_generate
	google-chrome-stable test/tests-lcov/index.html
