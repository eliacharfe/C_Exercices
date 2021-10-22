#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // for read/write/close
#include <sys/types.h> /* standard system types */
#include <netinet/in.h> /* Internet address structures */
#include <sys/socket.h> /* socket interface functions */
#include <netdb.h>
#include <signal.h>

#define MAX_ARR 10
#define TRUE    1
#define IP1     1
#define IP2     3
#define PORT1   2
#define PORT2   4
#define FALSE   0
//-----------------------------------
void run_front_end(int my_socket, int my_socket2);
void sig_terminate(int status);
void exit_error(char str[]);
//-------------------------------
int end = FALSE;
//--------------------------------
int main(int argc, char *argv[])
{
	signal(SIGINT, sig_terminate);
	int rc, rc2, my_socket, my_socket2;
	struct addrinfo con_kind, *addr_info_res ,
	con_kind2, *addr_info_res2 ;
	if (argc != 5) {
		fprintf(stderr, "Missing server name \n");
		exit(EXIT_FAILURE) ;
	}
	memset(&con_kind, 0, sizeof(con_kind)) ;
	memset(&con_kind2, 0, sizeof(con_kind2)) ;
	con_kind.ai_family = AF_UNSPEC ;
	con_kind2.ai_family = AF_UNSPEC ;
	con_kind.ai_socktype = SOCK_STREAM ;
	con_kind2.ai_socktype = SOCK_STREAM ;
	if ((rc = getaddrinfo(argv[IP1], argv[PORT1],	&con_kind, &addr_info_res) != 0)){
		fprintf(stderr, "(getaddrinfo() failed %s\n", gai_strerror(rc)) ;
		exit(EXIT_FAILURE) ;
	}
	if ((rc2 = getaddrinfo(argv[IP2],argv[PORT2],&con_kind2, &addr_info_res2) != 0)){
		fprintf(stderr, "(getaddrinfo2() failed %s\n", gai_strerror(rc2)) ;
		exit(EXIT_FAILURE) ;
	}
	my_socket = socket(addr_info_res->ai_family,
			addr_info_res->ai_socktype, // SOCK_STREAM
			addr_info_res->ai_protocol);
	my_socket2 = socket(addr_info_res2->ai_family,
			addr_info_res2->ai_socktype, // SOCK_STREAM
			addr_info_res2->ai_protocol);
	if (my_socket < 0 || my_socket2 < 0)
		exit_error("allocation_failed");
	rc = connect(my_socket,addr_info_res ->ai_addr,addr_info_res ->ai_addrlen);
	rc2= connect(my_socket2,addr_info_res2->ai_addr,addr_info_res->ai_addrlen);
	if (rc || rc2)
		exit_error("connect failed");

	run_front_end(my_socket, my_socket2);

	return EXIT_SUCCESS;
}
//----------------------------------
void run_front_end(int my_socket, int my_socket2)
{
	int arr[MAX_ARR], num, num2, res;
	char c;
	printf("Enter g or d: ");
	while (scanf("%c", &c))
	{
		//if (end)
		//	freeaddrinfo(addr_info_res) ;
		switch(c)
		{
		case 'g':
			scanf("%d %d", &num, &num2);
			write(my_socket, &num , sizeof(int)) ;
			write(my_socket, &num2 , sizeof(int)) ;
			read(my_socket, &res , sizeof(int)) ;
			printf("%d\n", res);
			break;
		case 'd':
			scanf ("%d" , &num);
			write(my_socket2, &num , sizeof(int)) ;
			read(my_socket2, arr , MAX_ARR) ;
			for(int i = 0 ; arr[i] != 0 ; i++)
			{
				printf("%d ", arr[i]);
				arr[i] = 0; // reset to zero
			}
			printf("\n");
			break;
		}
	}
}
//-------------------------------------
void sig_terminate(int status)
{ end = TRUE; }
//--------------------------------------
void exit_error(char str[])
{
	perror(str);
	exit (EXIT_FAILURE);
}

