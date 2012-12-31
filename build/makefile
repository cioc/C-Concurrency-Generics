all: examples

lib_objs = thread_pool.o producer_consumer.o

example_names = sleep echo_server echo_client

libs = -lpthread
cflags = -std=gnu99 -Werror

lib: $(lib_objs)

thread_pool.o: ../src/thread_pool.c
	gcc $(cflags) -c ../src/thread_pool.c $(libs)

producer_consumer.o: ../src/producer_consumer.c
	gcc $(cflags) -c ../src/producer_consumer.c $(libs)

examples: lib sleep echo 

echo: echo_server echo_client

sleep: $(lib_objs) sleep.o
	gcc $(lib_objs) sleep.o -o sleep $(libs) 

echo_server: $(lib_objs) echo_server.o
	gcc $(lib_objs) echo_server.o -o echo_server $(libs)

echo_client: echo_client.o
	gcc echo_client.o -o echo_client

echo_server.o: ../examples/echo_server.c
	gcc $(cflags) -c ../examples/echo_server.c $(libs)

echo_client.o: ../examples/echo_client.c
	gcc $(cflags) -c ../examples/echo_client.c

sleep.o: ../examples/sleep.c
	gcc $(cflags) -c ../examples/sleep.c $(libs)

clean:
	rm -rf *.o $(example_names)
