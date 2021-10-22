#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>

#define MAX_ARR 10
#define TRUE  1
#define FALSE 0
//--------------------------------------
void deal_with_client(int main_socket,fd_set rfd,  fd_set copy_rfd,
		struct sockaddr_storage her_addr, socklen_t her_addr_size,
		struct addrinfo *addr_info_res);
void decompose(int num, int arr[]);
void sig_terminate(int status);
void exit_error(char str[]);
//----------------------------
int end = FALSE;
//---------------------------------
int main(int argc, char *argv[])
{
	signal(SIGINT, sig_terminate);
	int rc2,  main_socket2;
	fd_set rfd2, copy_rfd2;
	struct sockaddr_storage her_addr2;
	socklen_t her_addr_size2;
	struct addrinfo con_kind2, *addr_info_res2 ;
	memset(&con_kind2, 0, sizeof con_kind2) ;
	con_kind2.ai_family = AF_UNSPEC ;
	con_kind2.ai_socktype = SOCK_STREAM ;
	con_kind2.ai_flags = AI_PASSIVE ; // system will fill my IP

	if ((rc2 = getaddrinfo(NULL, argv[1], &con_kind2, &addr_info_res2)) != 0) {
		fprintf(stderr, "(getaddrinfo() failed %s\n", gai_strerror(rc2)) ;
		exit(EXIT_FAILURE) ;
	}
	main_socket2 = socket(addr_info_res2 -> ai_family,
			addr_info_res2 -> ai_socktype,
			addr_info_res2 -> ai_protocol);
	if (main_socket2 < 0)
		exit_error("socket: allocation failed");
	rc2 = bind(main_socket2, addr_info_res2-> ai_addr,
			addr_info_res2-> ai_addrlen);
	if (rc2)
		exit_error("bind failed");
	rc2 = listen(main_socket2, 5);
	if (rc2)
		exit_error("listen failed");
	her_addr_size2 = sizeof(her_addr2);
	FD_ZERO(&rfd2);
	FD_SET(main_socket2, &rfd2);

	deal_with_client(main_socket2,rfd2,copy_rfd2,
			her_addr2, her_addr_size2,addr_info_res2);

	freeaddrinfo(addr_info_res2) ;
	return EXIT_SUCCESS;
}
//-------------------------------------------
void deal_with_client(int main_socket,fd_set rfd,  fd_set copy_rfd,
		struct sockaddr_storage her_addr, socklen_t her_addr_size,
		struct addrinfo *addr_info_res)
{
	int serving_socket, fd;
	int arr[MAX_ARR], num;
	while (TRUE)
	{
		copy_rfd = rfd;
		select(getdtablesize(), &copy_rfd, NULL, NULL,(struct timeval *) NULL);
		if (FD_ISSET(main_socket, &copy_rfd))
		{
			serving_socket = accept(main_socket, (struct sockaddr *)&her_addr,
					&her_addr_size);
			if (serving_socket >= 0)
				FD_SET(serving_socket, &rfd);
		}
		for (fd = main_socket +1; fd < getdtablesize(); fd++)
		{
			memset(arr, 0, MAX_ARR * sizeof(int));
			if (end)
				freeaddrinfo(addr_info_res) ;
			if (FD_ISSET(fd, &copy_rfd))
			{
				read(serving_socket, &num, sizeof(int));
				decompose(num, arr);
				write(serving_socket, arr, MAX_ARR);
			}
		}
	}
}
//---------------------------------------------------------
void decompose(int num, int arr[])
{
	if (num < 2)
	{
		arr[0] = num;
		return;
	}
	int div = 2;
	int counter = 0;
	while (num > 1)
	{
		int i = 0;
		while (num % div == 0)
		{
			++i;
			num /= div;
		}
		if (i != 0)
		{
			arr[counter] = div;
			counter++;
			if (i != 1)
				arr[counter] = 1;
		}
		div = (div == 2) ? 3 : div + 2;
	}
}
//-------------------------------------
void sig_terminate(int status)
{ end = TRUE;	}
//--------------------------------------------
void exit_error(char str[])
{
	perror(str);
	exit (EXIT_FAILURE);
}

