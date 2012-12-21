all: examples

lib_objs = thread_pool.o producer_consumer.o

libs = -lpthread
cflags = -std=gnu99 -Werror

lib: $(lib_objs)

thread_pool.o: ../src/thread_pool.c
	gcc $(cflags) -c ../src/thread_pool.c $(libs)

producer_consumer.o: ../src/producer_consumer.c
	gcc $(cflags) -c ../src/producer_consumer.c $(libs)

examples: lib echo

echo: $(lib_objs) main.o
	gcc $(lib_objs) main.o -o echo $(libs) 

main.o: ../examples/main.c
	gcc $(cflags) -c ../examples/main.c $(libs)

clean:
	rm -rf *.o echo 
