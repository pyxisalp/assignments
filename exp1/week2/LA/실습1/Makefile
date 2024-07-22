cc = gcc
cflags = -W -g
target = animal

objects = dog.o blackcow.o turtle.o main.o

$(target) : $(objects)
	$(cc) $(cflags) -o $(target) $(objects)

$.o : %.c
	$(cc) $(cflags) -c -o $@ $<

.PHONY : clean
clean :
	rm $(target) $(objects)
