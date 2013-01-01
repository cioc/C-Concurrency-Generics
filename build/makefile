all: examples

lib_objs = thread_pool.o producer_consumer.o net_support.o

src_path = ../src
exa_path = ../examples

example_names = sleep echo_server echo_client

libs = -lpthread
cflags = -std=gnu99 -Werror

lib: $(lib_objs)

thread_pool.o: $(src_path)/thread_pool.c
	gcc $(cflags) -c $(src_path)/thread_pool.c $(libs)

producer_consumer.o: $(src_path)/producer_consumer.c
	gcc $(cflags) -c $(src_path)/producer_consumer.c $(libs)

net_support.o: $(src_path)/net_support.c
	gcc $(cflags) -c $(src_path)/net_support.c $(libs)

examples: lib sleep echo 

echo: echo_server echo_client

sleep: $(lib_objs) sleep.o
	gcc $(lib_objs) sleep.o -o sleep $(libs) 

echo_server: $(lib_objs) echo_server.o
	gcc $(lib_objs) echo_server.o -o echo_server $(libs)

echo_client: echo_client.o
	gcc $(lib_objs) echo_client.o -o echo_client $(libs)

echo_server.o: $(exa_path)/echo_server.c
	gcc $(cflags) -c $(exa_path)/echo_server.c $(libs)

echo_client.o: $(exa_path)/echo_client.c
	gcc $(cflags) -c $(exa_path)/echo_client.c

sleep.o: $(exa_path)/sleep.c
	gcc $(cflags) -c $(exa_path)/sleep.c $(libs)

clean:
	rm -rf *.o $(example_names)
