/* ex6a.c
 *    Socket
 * =============================================
 * ==== Written by: Eliachar Feig. ID: 302278338 ===
 * =============================================
 * This program create a socket for communicates from sever to clients
 * and builds array of 1000 and insert to it randomally from 0 to 200000.
 * Proceess will run until he erase all the array according to the numbers
 * sent by the clients. each client will run in an infinite loop and will send
 * to server randomly a number from 0 to 200000, the server get it and send 0
 * if not found and 1 if found, that's how the client can calculate how many
 * number he sent that made deletion in the server's array.
 *  When the array is empty the server exit loop and send to clients process -1
 *  which tell them to terminate. Server will send -1 to client to tell them
 *  to terminate program then  terminate then the program
 *   The client gets in argument 1, 2, or 3 (each child another one) which will
 *   be the seed, the IP of server process (on which computer he is working on),
 *   and the PORT that the server is listening to.
 *
 *   No input
 * Output : Server: how many numbers got from children, how many he deleted and
 *                  how much time took for do the job
 *          Client: how many number send and how many caused a delete
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <time.h>
//-----------------------------------
#define MAX_ARR 	1000
#define MAX_RAND    200000
#define TRUE 		1
#define FALSE 		0
#define NUM_CLIENTS 3
#define END 		-1
//---------------------------
void dealing_with_clients(int arr[],int main_socket, fd_set rfd,
		     struct sockaddr_storage her_addr, socklen_t her_addr_size);
void build_array(int arr[]);
int  delete_from_arr(int arr[], int max_arr, int num_to_del);
void quickSort(int arr[], int low, int high);
int partition (int arr[], int low, int high);
void swap(int* i, int* j);
bool bin_search(int arr[], int num);
//----------------------------------
int main(int argc, char *argv[])
{
	int rc,  main_socket;
	fd_set rfd;
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
	if (main_socket < 0) {
		perror("socket: allocation failed");
		exit(EXIT_FAILURE) ;
	}
	rc = bind(main_socket, addr_info_res-> ai_addr,
			addr_info_res-> ai_addrlen);
	if (rc) {
		perror("bind failed");
		exit(EXIT_FAILURE) ;
	}
	rc = listen(main_socket, 5);
	if (rc) {
		perror("listen failed");
		exit(EXIT_FAILURE) ;
	}

	int arr[MAX_ARR];
    build_array(arr);
	quickSort(arr, 0,  MAX_ARR-1);//sort array

	her_addr_size = sizeof(her_addr);
	FD_ZERO(&rfd);
	FD_SET(main_socket, &rfd);

	dealing_with_clients(arr, main_socket, rfd, her_addr, her_addr_size);

	freeaddrinfo(addr_info_res) ;

	return(EXIT_SUCCESS) ;
}
//-------------------------------------------------------------------
void dealing_with_clients(int arr[],int main_socket, fd_set rfd,
		     struct sockaddr_storage her_addr, socklen_t her_addr_size)
{
    fd_set copy_rfd;
	int counter_clients = 0,found = FALSE, num,  max_arr = MAX_ARR,
			num_of_father_get = 0, num_of_del = 0, serving_socket, fd;
	time_t time1;
	while (TRUE)
	{
		copy_rfd = rfd;
		select(getdtablesize(), &copy_rfd, NULL, NULL, (struct timeval *) NULL);
		if (FD_ISSET(main_socket, &copy_rfd))
		{
			serving_socket = accept(main_socket, (struct sockaddr *)&her_addr,&her_addr_size);
			if (serving_socket >= 0)
				FD_SET(serving_socket, &rfd);
			counter_clients++;
		}
		if (counter_clients != NUM_CLIENTS)
			continue;
	    time1 = time(NULL);
		for (fd = main_socket +1; fd < getdtablesize(); fd++)
		{
			if (FD_ISSET(fd, &copy_rfd))
			{
				read(fd, &num, sizeof(int));
				num_of_father_get++;
				if (bin_search(arr, num))
				{
					max_arr = delete_from_arr(arr,max_arr, num);
					num_of_del++;
					found = TRUE;
				}
				if (max_arr == 0)
				{
					found = END;
					write(fd, &found, sizeof(int));
				}
				write(fd, &found, sizeof(int));
				if (found != END)
				found = FALSE;
			}
		}
		if (found == END)
			break;
	}
	time_t time2 = time(NULL);
	time_t time = time2 - time1;
	found = END;
	write(fd, &found, sizeof(int));
	write(fd, &found, sizeof(int));
	write(fd, &found, sizeof(int));
	printf("%d %d %lf\n",num_of_father_get,num_of_del,(double)time);
}
//-------------------------------------------------------
void build_array(int arr[])
{
	for (int i = 0; i < MAX_ARR; i++) // build array, max = 50000
		arr[i] = rand()% MAX_RAND;// 0 -200000
}
//--------------------------------------------------------
int delete_from_arr(int arr[], int max_arr, int num_to_del)
{
	int i, j;
	for (i = 0; i < max_arr; i++)
		if (arr[i] == num_to_del)
			break;
	if (i <= max_arr)
	{
		max_arr--;
		for (j = i; j < max_arr; j++)
			arr[j] = arr[j+1];
	}
	return max_arr;
}
//---------------------------------------------------------------------------
bool bin_search(int arr[], int num)
{
	int first = 0;
	int last = MAX_ARR - 1;
	int middle = (first + last)/2;
	while (first <= last)
	{
		if (arr[middle] < num)
			first = middle + 1;
		else if (arr[middle] == num) {
			return true;
		}
		else
			last = middle - 1;
		middle = (first + last)/2;
	}
	return false;
}
//-------------------------------------------------
void quickSort(int arr[], int low, int high)
{
	if (low < high)
	{
		int pivot = partition(arr, low, high);
		quickSort(arr, low, pivot - 1);
		quickSort(arr, pivot + 1, high);
	}
}
//------------------------------------------------
int partition (int arr[], int low, int high)
{
	int pivot = arr[high], j;
	int i = (low - 1);
	for (j = low; j <= high - 1; j++)
	{
		if (arr[j] < pivot)
		{
			i++;
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[high]);
	return (i + 1);
}
//--------------------------------------
void swap(int* i, int* j)
{
	int temp = *i;
	*i = *j;
	*j = temp;
}
