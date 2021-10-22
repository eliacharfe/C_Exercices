/* ./ex6b
 * Socket: Common Greatest Divisor + Decomposition
 * ========================================================
 * === Written by: Eliachar Feig. id: 302278338 ===
 * =======================================================
 * This program run 3 different process that read and write into socket.
 * ex6b1 and ex6b2 will be 2 different servers, ex6b3 is the client process.
 * ex6b3: reads from the user in infinite loop a char:
 *  if the char is 'g':
 * then ask for 2 integers and send it through socket that reads ex6b1
 * which calculates the greatest common divisor (gcd) and return the answer.
 * if the char is 'd':
 * then ask for an integer from the user and send it to the other socket
 * to ex6b2 which decompose the number to primes numbers and return the
 * answer in an array of max size 10.
 * ex6b1: allocate socket, then in infinte loop reads 2 integers from
 * client and return the gcd, if get signal SIGINT - free socket.
 * ex6b2: allocate socket, get an integer from and decompose it to
 * prime numbers
 *
 * All processes when get signal SIGINT free the accurate socket
 * they works on
 *
 * Input: in infinite loop: scan a char:
 *  if 'g': then get 2 integers
 *  if 'd': then get 1 integers
 *
 *  Output: according to the char:
 *  if 'g': gcd
 *  if 'd': decomposition of primes
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // for memset
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>

#define MY_PORT "3879" // A String!
#define MAX_ARR 	500
#define MAX_RAND    500
#define TRUE 		1
#define FALSE 		0
#define NUM_CLIENTS 3
#define END 		-1
//---------------------------------------
void deal_with_client(int main_socket,fd_set rfd, fd_set copy_rfd,
		struct sockaddr_storage her_addr, socklen_t her_addr_size,
		struct addrinfo *addr_info_res);
int max_div(int num1, int num2);
void sig_terminate(int status);
void exit_error(char str[]);
//---------------------------------
int end = FALSE;
//----------------------------------
int main(int argc, char *argv[])
{
	signal(SIGINT, sig_terminate);
	int rc,  main_socket;
	fd_set rfd, copy_rfd;
	struct sockaddr_storage her_addr;
	socklen_t her_addr_size;
	struct addrinfo con_kind, *addr_info_res ;
	memset(&con_kind, 0, sizeof con_kind) ;
	con_kind.ai_family = AF_UNSPEC ;
	con_kind.ai_socktype = SOCK_STREAM ;
	con_kind.ai_flags = AI_PASSIVE ; // system will fill my IP

	if ((rc = getaddrinfo(NULL,	argv[1], &con_kind, &addr_info_res)) != 0) {
		fprintf(stderr, "(getaddrinfo() failed %s\n", gai_strerror(rc)) ;
		exit(EXIT_FAILURE) ;
	}
	main_socket = socket(addr_info_res -> ai_family,
			addr_info_res -> ai_socktype,
			addr_info_res -> ai_protocol);
	if (main_socket < 0)
		exit_error("socket: allocation failed");
	rc = bind(main_socket, addr_info_res-> ai_addr,
			addr_info_res-> ai_addrlen);
	if (rc)
		exit_error("bind failed");

	rc = listen(main_socket, 5);
	if (rc)
		exit_error("listen failed");
	her_addr_size = sizeof(her_addr);
	FD_ZERO(&rfd);
	FD_SET(main_socket, &rfd);

	deal_with_client(main_socket, rfd,copy_rfd, her_addr, her_addr_size, addr_info_res);


	return(EXIT_SUCCESS) ;
}
//-------------------------------------------------------------------
void deal_with_client(int main_socket,fd_set rfd,  fd_set copy_rfd,
		struct sockaddr_storage her_addr, socklen_t her_addr_size,
		struct addrinfo *addr_info_res)
{
	int serving_socket, fd;
	int num, num2, gcd;
	while (1)
	{
		copy_rfd = rfd;
		select(getdtablesize(), &copy_rfd, NULL, NULL, (struct timeval *) NULL);
		if (FD_ISSET(main_socket, &copy_rfd))
		{
			serving_socket = accept(main_socket, (struct sockaddr *)&her_addr,&her_addr_size);
			if (serving_socket >= 0)
				FD_SET(serving_socket, &rfd);
		}

		for (fd = main_socket +1; fd < getdtablesize(); fd++)
		{
			if (end)
				freeaddrinfo(addr_info_res) ;
			if (FD_ISSET(fd, &copy_rfd))
			{
				read(fd, &num, sizeof(int));
				read(fd, &num2, sizeof(int));
				gcd = max_div(num, num2);
				write(fd, &gcd, sizeof(int));

			}
		}
	}
}
//--------------------------------------------
int max_div(int num1, int num2)
{
	int gcd;
	for(int div = 1; div <= num1 && div <= num2; ++div)
	{
		if(num1 % div == 0 && num2 % div == 0)
			gcd = div;
	}
	return gcd;
}
//-------------------------------------
void sig_terminate(int status)
{ end = TRUE; }
//--------------------------------------------
void exit_error(char str[])
{
	perror(str);
	exit (EXIT_FAILURE);
}

