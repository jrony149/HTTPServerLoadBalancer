loadbalancer2 : loadbalancer2.o List.o
	gcc -g -o loadbalancer2 loadbalancer2.o List.o -pthread -Wall -Wextra -Wpedantic -Wshadow -O2

loadbalancer2.o : loadbalancer2.c List.h
	gcc -g -Og -c -Wall -Wextra -Wpedantic -Wshadow -O2 loadbalancer2.c

ListClient: ListClient.o List.o
	gcc -o ListClient ListClient.o List.o -Wall -Wextra -Wpedantic -Wshadow -O2

ListClient.o : ListClient.c List.h
	gcc -g -Og -c -Wall -Wextra -Wpedantic -Wshadow -O2 ListClient.c

List.o : List.c List.h
	gcc -g -Og -c -Wall -Wextra -Wpedantic -Wshadow -O2 List.c

clean :
	rm -f loadbalancer2 ListClient loadbalancer2.o ListClient.o List.o
