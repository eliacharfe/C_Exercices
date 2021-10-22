#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
//----------------------
#define MAX_RAND 200000
#define TRUE 1
#define END -1
//---------------------------------
void dealing_with_server(int my_socket, char argv[1]);
//----------------------------------
int main(int argc, char *argv[])
{
	int rc, my_socket;
	struct addrinfo con_kind, *addr_info_res ;
	if (argc != 4) {
		fprintf(stderr, "Missing server name \n");
		exit(EXIT_FAILURE) ;
	}
	memset(&con_kind, 0, sizeof(con_kind)) ;
	con_kind.ai_family = AF_UNSPEC ;
	con_kind.ai_socktype = SOCK_STREAM ;
	if ((rc = getaddrinfo(argv[2], argv[3], &con_kind, &addr_info_res) != 0)){
		fprintf(stderr, "(getaddrinfo() failed %s\n", gai_strerror(rc)) ;
		exit(EXIT_FAILURE) ;
	}
	my_socket = socket(addr_info_res->ai_family,
			addr_info_res->ai_socktype, // SOCK_STREAM
			addr_info_res->ai_protocol);
	if (my_socket < 0) {
		perror("socket: allocation failed");
		exit(EXIT_FAILURE) ;
	}
	rc = connect(my_socket, addr_info_res ->ai_addr,
			addr_info_res ->ai_addrlen) ;
	if (rc) {
		perror("connect failed:");
		exit(EXIT_FAILURE) ;
	}

	dealing_with_server(my_socket, argv[1]);

	close(my_socket);
	freeaddrinfo(addr_info_res) ;
	return EXIT_SUCCESS;
}
//--------------------------------
void dealing_with_server(int my_socket, char *str)
{
	int num , found, child_send = 0, father_delete = 0;
	srand(atoi(str));
	while (found != END)
	{
		num = rand() % MAX_RAND;
		write(my_socket, &num, sizeof(int)) ;
		child_send++;
		read(my_socket, &found, sizeof(int)) ;
		if (found == TRUE)
			father_delete++;
	}
	printf ("child send: %d caused delete: %d ", child_send, father_delete);
}





