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

linked_list.o: src/linked_list.c src/linked_list.h demo/common.h
	$(C_COMPILER) $(C_OPTIONS) -c src/linked_list.c


## WEBSTORE ##

webstore: frontend.o backend.o utils.o hash_table.o inlupp_linked_list.o

frontend.o: demo/frontend.c demo/frontend.h demo/backend.h demo/hash_table.h demo/inlupp_linked_list.h demo/utils.h demo/common.h src/refmem.h
	$(C_COMPILER) $(C_OPTIONS) -c demo/frontend.c

backend.o: demo/backend.c demo/frontend.h demo/backend.h demo/hash_table.h demo/inlupp_linked_list.h demo/utils.h demo/common.h src/refmem.h
	$(C_COMPILER) $(C_OPTIONS) -c demo/backend.c

inlupp_linked_list.o: demo/inlupp_linked_list.c demo/inlupp_linked_list.h demo/common.h
	$(C_COMPILER) $(C_OPTIONS) -c demo/inlupp_linked_list.c

hash_table.o: demo/hash_table.c demo/hash_table.h demo/inlupp_linked_list.h demo/common.h src/refmem.h
	$(C_COMPILER) $(C_OPTIONS) -c demo/hash_table.c

utils.o: demo/utils.c demo/utils.h
	$(C_COMPILER) $(C_OPTIONS) -c demo/utils.c


## TEST ##

lib_for_tests.o: test/lib_for_tests.c test/lib_for_tests.h src/refmem.h allocate.o
	$(C_COMPILER) $(C_OPTIONS) -c test/lib_for_tests.c



################### TEST RUNS ######################

.PHONY: tests
test: src
	./test/tests

memtest: src
	$(C_VALGRIND) ./test/tests

crayparty_val_test: linked_list.o cleanup.o allocate.o src/crayparty.c cascade.o lib_for_tests.o
	$(C_COMPILER) $(C_OPTIONS) $^ -o test/crayparty
	$(C_VALGRIND) test/crayparty

example: test/example.c allocate.o cascade.o lib_for_tests.o cleanup.o linked_list.o
	$(C_COMPILER) $(C_LCOV) $(C_OPTIONS) $^ -o test/example $(CUNIT_LINK)
	$(C_VALGRIND) test/example

ll_test: demo/ll_test.c hash_table.o allocate.o cleanup.o cascade.o lib_for_tests.o inlupp_linked_list.o linked_list.o
	$(C_COMPILER) $(C_LCOV) $(C_OPTIONS) $^ -o test/ll_test $(CUNIT_LINK)
	$(C_VALGRIND) ./test/ll_test

webstore_test: demo/tests.c hash_table.o backend.o utils.o allocate.o cleanup.o cascade.o lib_for_tests.o inlupp_linked_list.o linked_list.o
	$(C_COMPILER) $(C_LCOV) $(C_OPTIONS) $^ -o test/webstore_test $(CUNIT_LINK)
	$(C_VALGRIND) ./test/webstore_test

inlupp_unit_tests: demo/unit_tests.c hash_table.o backend.o utils.o allocate.o cleanup.o cascade.o lib_for_tests.o inlupp_linked_list.o linked_list.o
	$(C_COMPILER) $(C_LCOV) $(C_OPTIONS) $^ -o test/unit_test $(CUNIT_LINK)
	./test/unit_test

val_inlupp_unit_tests: demo/unit_tests.c hash_table.o backend.o utils.o allocate.o cleanup.o cascade.o lib_for_tests.o inlupp_linked_list.o linked_list.o
	$(C_COMPILER) $(C_LCOV) $(C_OPTIONS) $^ -o test/unit_test $(CUNIT_LINK)
	$(C_VALGRIND) ./test/unit_test

############# LCOV ##################

lcov_generate: memtest
	lcov --capture --rc lcov_branch_coverage=1 --directory . --output-file test/tests.info

lcov: lcov_generate
	genhtml --rc genhtml_branch_coverage=1 test/tests.info -o test/tests-lcov

lcov_open: lcov_generate
	make lcov
	google-chrome-stable test/tests-lcov/index.html

lcov_new: test/tests.c src/allocate.c src/cleanup.c src/cascade.c lib_for_tests.o linked_list.o
	$(C_COMPILER) --coverage $? -o test/tests $(CUNIT_LINK)
	./test/tests
#	gcov -abcfu src/allocate.c src/cleanup.c src/cascade.c test/tests.c
	lcov -c -d . -o test/tests.info
	genhtml test/tests.info -o test/tests-lcov
	google-chrome-stable test/tests-lcov/index.html

################# GPROF #################

gprof: test/example.c allocate.o cascade.o lib_for_tests.o cleanup.o linked_list.o
	gcc -pg $^ -o test/gprof
	test/gprof
	gprof test/gprof | less


clean:	
	rm *.o *.gcno *.gcda gmon.out ./test/tests ./test/crayparty ./test/example ./test/ll_test ./test/webstore_test ./test/unit_test ./test/gprof ./test/*.gcno ./src/*.gch ./demo/*.gcno ./demo/*.gcda ./demo/*.gch
