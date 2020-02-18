
compile_webstore: frontend.c backend.c utils.c hash_table.c inlupp_linked_list.c ../src/cleanup.c ../src/allocate.c ../src/cascade.c ../test/lib_for_tests.c ../src/linked_list.c
	gcc --coverage -Wall -pedantic -g frontend.c backend.c utils.c hash_table.c inlupp_linked_list.c ../src/cleanup.c ../src/allocate.c ../src/cascade.c ../test/lib_for_tests.c ../src/linked_list.c

run: compile_webstore
	./a.out

compile_tests: tests.c backend.c utils.c hash_table.c inlupp_linked_list.c ../src/cleanup.c ../src/allocate.c ../src/cascade.c ../test/lib_for_tests.c ../src/linked_list.c 
	gcc -Wall -pedantic -g tests.c backend.c utils.c hash_table.c inlupp_linked_list.c ../src/cleanup.c ../src/allocate.c ../src/cascade.c ../test/lib_for_tests.c ../src/linked_list.c -lcunit

only_test: compile_tests
	./a.out

mem: compile_webstore
	valgrind --leak-check=full --show-leak-kinds=all ./a.out


test: compile_tests
	valgrind -v --leak-check=full --track-origins=yes ./a.out 

compile_lcov: test
	lcov -c -d . -o tests.info 

lcov: compile_lcov
	genhtml tests.info -o tests-lcov




ll_test: hash_table.h linked_list.h hash_table.c linked_list.c ll_test.c
	gcc --coverage -Wall -pedantic -g hash_table.h linked_list.h hash_table.c linked_list.c ll_test.c -o tests -lcunit -pg


the_ht_test: hash_table.h linked_list.h hash_table.c linked_list.c ht_test.c
	gcc --coverage -Wall -pedantic -g hash_table.h linked_list.h hash_table.c linked_list.c ht_test.c -o tests -lcunit -pg


u_tests: hash_table.h linked_list.h hash_table.c linked_list.c unit_tests.c
	gcc --coverage -Wall -pedantic -g hash_table.h linked_list.h hash_table.c linked_list.c unit_tests.c -o tests -lcunit -pg

linked_list_test: ll_test
	valgrind -v --leak-check=full --track-origins=yes ./a.out 

hash_table_test: the_ht_test
	valgrind -v --leak-check=full --track-origins=yes ./a.out 

ht_and_ll_test: u_tests
	valgrind -v --leak-check=full --track-origins=yes ./a.out 
