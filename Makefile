all: trie.o

trie.o: trie.c trie.h
	$(CC) $(CFLAGS) -o trie.o -c trie.c

test: trie.o test.o
	$(CC) $(CFLAGS) -o test test.o trie.o

test.o: test.c trie.h
	$(CC) $(CFLAGS) -o test.o -c test.c

run_test: test
	./test

clean:
	rm -rf *.o test
