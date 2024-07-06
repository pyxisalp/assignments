cc = gcc

all: hw1_naive hw1_kmp

hw1_naive: hw1_naive.c
	$(cc) -o $@ $<

hw1_kmp: hw1_kmp.c
	$(cc) -o $@ $<

.PHONY: clean
clean:
	rm hw1_naive hw1_kmp
