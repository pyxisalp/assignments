cc = g++
target = main
objects = main.o array.o rangearray.o

$(cc): $(objects) Array.h RangeArray.h
	$(cc) -o $(target) $(objects)

.PHONY: clean
clean:
	rm $(objects) $(target)

