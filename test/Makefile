.PHONY: test

CFLAGS=-I ../src -Wall -g -DSENTINEL #(-DSENTINEL 주석 해제함)

test: test-rbtree
	./test-rbtree
	valgrind ./test-rbtree

test-rbtree: test-rbtree.o ../src/rbtree.o

../src/rbtree.o:
	$(MAKE) -C ../src rbtree.o

clean:
	rm -f test-rbtree *.o
