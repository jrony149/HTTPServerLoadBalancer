
httpserver4 : httpserver4.o List.o
	gcc -g -o httpserver4 httpserver4.o List.o -pthread -Wall -Wextra -Wpedantic -Wshadow -O2

httpserver4.o : httpserver4.c List.h
	gcc -g -Og -c -Wall -Wextra -Wpedantic -Wshadow -O2 httpserver4.c

ListClient: ListClient.o List.o
	gcc -o ListClient ListClient.o List.o -Wall -Wextra -Wpedantic -Wshadow -O2

ListClient.o : ListClient.c List.h
	gcc -g -Og -c -Wall -Wextra -Wpedantic -Wshadow -O2 ListClient.c

List.o : List.c List.h
	gcc -g -Og -c -Wall -Wextra -Wpedantic -Wshadow -O2 List.c

clean :
	rm -f httpserver4 ListClient httpserver4.o ListClient.o List.o