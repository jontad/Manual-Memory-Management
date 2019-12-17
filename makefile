C_COMPILER     = gcc
C_OPTIONS      = -Wall -ggdb -pedantic -g
C_LINK_OPTIONS = -lm 
CUNIT_LINK     = -lcunit
C_LCOV 	       = --coverage 	


clean:	
	rm *.o ./test/tests *.gcno *.gcda

################ COMPILES ###################

linked_list.o: src/linked_list.c src/linked_list.h src/common.h
	$(C_COMPILER) $(C_OPTIONS) -c src/linked_list.c

cascade.o: src/cascade.c src/refmem.h
	$(C_COMPILER) $(C_OPTIONS) -c src/cascade.c

cleanup.o: src/cleanup.c src/refmem.h
	$(C_COMPILER) $(C_OPTIONS) -c src/cleanup.c

allocate.o: src/allocate.c src/refmem.h src/linked_list.h
	$(C_COMPILER) $(C_OPTIONS) -c src/allocate.c

compile: allocate.o cleanup.o cascade.o linked_list.o

test_compile: test/tests.c allocate.o cleanup.o cascade.o linked_list.o test/lib_for_tests.c
	$(C_COMPILER) $(C_LCOV) $(C_OPTIONS) $^ -o test/tests $(CUNIT_LINK)


################### TEST RUNS ######################

tests: test_compile
	./test/tests	

val_tests: test_compile
	valgrind --leak-check=full ./test/tests

crayparty_val_test: linked_list.o cleanup.o allocate.o src/crayparty.c
	$(C_COMPILER) $(C_OPTIONS) $^ -o test/crayparty
	valgrind --leak-check=full test/crayparty


############# LCOV ##################

lcov_generate: tests
	lcov --capture --rc lcov_branch_coverage=1 --directory . --output-file test/tests.info

lcov: lcov_generate
	genhtml --rc genhtml_branch_coverage=1 test/tests.info -o test/tests-lcov

lcov_open: lcov_generate
	google-chrome-stable test/tests-lcov/index.html
