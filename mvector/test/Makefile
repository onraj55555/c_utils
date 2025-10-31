1-int:
	gcc 1-int.c -o 1-int -I../../mvector -I../../allocator

2-big-struct-on-stack:
	gcc 2-big-struct-on-stack.c -o 2-big-struct-on-stack -I../../mvector -I../../allocator

run: 1-int 2-big-struct-on-stack
	./1-int
	./2-big-struct-on-stack

test: run
	rm -f 1-int 2-big-struct-on-stack

clean:
	rm -f 1-int 2-big-struct-on-stack
