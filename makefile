C_COMPILER	    = gcc
C_OPTIONS	      = -Wall -ggdb -pedantic -g
C_LINK_OPTIONS	= -lm
CUNIT_LINK	    = -lcunit
C_LCOV	    	  = --coverage
C_VALGRIND  	  = valgrind --leak-check=full --show-leak-kinds=all


################ COMPILES ###################

compile: src webstore

## SRC ##

src: test/tests.c allocate.o cleanup.o cascade.o lib_for_tests.o linked_list.o
	$(C_COMPILER) $(C_LCOV) $(C_OPTIONS) $^ -o test/tests $(CUNIT_LINK)

cascade.o: src/cascade.c src/refmem.h
	$(C_COMPILER) $(C_OPTIONS) -c src/cascade.c

cleanup.o: src/cleanup.c src/refmem.h
	$(C_COMPILER) $(C_OPTIONS) -c src/cleanup.c

allocate.o: src/allocate.c src/refmem.h src/linked_list.h test/lib_for_tests.h
	$(C_COMPILER) $(C_OPTIONS) -c  src/allocate.c

linked_list.o: src/linked_list.c src/linked_list.h inlupp2/common.h
	$(C_COMPILER) $(C_OPTIONS) -c src/linked_list.c


## WEBSTORE ##

webstore: frontend.o backend.o utils.o hash_table.o inlupp_linked_list.o

frontend.o: inlupp2/frontend.c inlupp2/frontend.h inlupp2/backend.h inlupp2/hash_table.h inlupp2/inlupp_linked_list.h inlupp2/utils.h inlupp2/common.h src/refmem.h
	$(C_COMPILER) $(C_OPTIONS) -c inlupp2/frontend.c

backend.o: inlupp2/backend.c inlupp2/frontend.h inlupp2/backend.h inlupp2/hash_table.h inlupp2/inlupp_linked_list.h inlupp2/utils.h inlupp2/common.h src/refmem.h
	$(C_COMPILER) $(C_OPTIONS) -c inlupp2/backend.c

inlupp_linked_list.o: inlupp2/inlupp_linked_list.c inlupp2/inlupp_linked_list.h inlupp2/common.h
	$(C_COMPILER) $(C_OPTIONS) -c inlupp2/inlupp_linked_list.c

hash_table.o: inlupp2/hash_table.c inlupp2/hash_table.h inlupp2/inlupp_linked_list.h inlupp2/common.h src/refmem.h
	$(C_COMPILER) $(C_OPTIONS) -c inlupp2/hash_table.c

utils.o: inlupp2/utils.c inlupp2/utils.h
	$(C_COMPILER) $(C_OPTIONS) -c inlupp2/utils.c


## TEST ##

lib_for_tests.o: test/lib_for_tests.c test/lib_for_tests.h src/refmem.h allocate.o
	$(C_COMPILER) $(C_OPTIONS) -c test/lib_for_tests.c



################### TEST RUNS ######################

.PHONY: tests
tests: src
	./test/tests

val_tests: src
	$(C_VALGRIND) ./test/tests

crayparty_val_test: linked_list.o cleanup.o allocate.o src/crayparty.c cascade.o lib_for_tests.o
	$(C_COMPILER) $(C_OPTIONS) $^ -o test/crayparty
	$(C_VALGRIND) test/crayparty

example: test/example.c allocate.o cascade.o lib_for_tests.o cleanup.o linked_list.o
	$(C_COMPILER) $(C_LCOV) $(C_OPTIONS) $^ -o test/example $(CUNIT_LINK)
	$(C_VALGRIND) test/example

ll_test: inlupp2/ll_test.c hash_table.o allocate.o cleanup.o cascade.o lib_for_tests.o inlupp_linked_list.o linked_list.o
	$(C_COMPILER) $(C_LCOV) $(C_OPTIONS) $^ -o test/ll_test $(CUNIT_LINK)
	$(C_VALGRIND) ./test/ll_test

webstore_test: inlupp2/tests.c hash_table.o backend.o utils.o allocate.o cleanup.o cascade.o lib_for_tests.o inlupp_linked_list.o linked_list.o
	$(C_COMPILER) $(C_LCOV) $(C_OPTIONS) $^ -o test/webstore_test $(CUNIT_LINK)
	./test/webstore_test

inlupp_unit_tests: inlupp2/unit_tests.c hash_table.o backend.o utils.o allocate.o cleanup.o cascade.o lib_for_tests.o inlupp_linked_list.o linked_list.o
	$(C_COMPILER) $(C_LCOV) $(C_OPTIONS) $^ -o test/unit_test $(CUNIT_LINK)
	./test/unit_test

val_inlupp_unit_tests: inlupp2/unit_tests.c hash_table.o backend.o utils.o allocate.o cleanup.o cascade.o lib_for_tests.o inlupp_linked_list.o linked_list.o
	$(C_COMPILER) $(C_LCOV) $(C_OPTIONS) $^ -o test/unit_test $(CUNIT_LINK)
	$(C_VALGRIND) ./test/unit_test

############# LCOV ##################

lcov_generate: val_tests
	lcov --capture --rc lcov_branch_coverage=1 --directory . --output-file test/tests.info

lcov: lcov_generate
	genhtml --rc genhtml_branch_coverage=1 test/tests.info -o test/tests-lcov

lcov_open: lcov_generate
	google-chrome-stable test/tests-lcov/index.html



################# GPROF #################

gprof: test/example.c allocate.o cascade.o lib_for_tests.o cleanup.o linked_list.o
	gcc -pg $^ -o test/gprof
	test/gprof
	gprof test/gprof | less


clean:	
	rm *.o *.gcno *.gcda gmon.out ./test/tests ./test/crayparty ./test/example ./test/ll_test ./test/webstore_test ./test/unit_test ./test/gprof ./test/*.gcno ./src/*.gch ./inlupp2/*.gcno ./inlupp2/*.gcda ./inlupp2/*.gch
