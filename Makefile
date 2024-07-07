exec = compiler.out
sources = ${wildcard src/*.c}
objects = ${sources:.c=.o}
flags = -g

$(exec): $(objects)
	gcc -g $(objects) $(flags) -o $(exec)

%.o: %.c include/%.h
	gcc -c $(flags) $< -o $@

clean:
	-del *.out
	-del /s *.o

clean_l:
	-rm *.out
	-rm *.o
	-rm src/*.o