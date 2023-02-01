all : sort.i sort.s sort.o sort
.PHONY : all
sort.i : sort.c
	gcc -E sort.c -o sort.i
sort.s : sort.i
	gcc -S sort.i -o sort.s
sort.o : sort.s
	as sort.s -o sort.o
sort : sort.o
	gcc sort.o -o sort
clean :
	rm sort.i sort.s sort.o sort