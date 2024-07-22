cc = gcc
target = s20181360
objects = main.o func_calc.o func_print.o

$(target) : $(objects)
	$(cc) -o $(target) $(objects)

$(objects) : hw.h

.PHONY : CLEAN
clean :
	rm $(target) $(objects)
