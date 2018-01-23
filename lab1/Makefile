flags = -std=c11 -pthread -O3

lab1: lab1.o
	cc $(flags) lab1.o -o lab1

.PHONY: tests
test: lab1
	python tests.py

.PHONY: clean
clean:
	-rm -f *.o
	-rm -f lab1