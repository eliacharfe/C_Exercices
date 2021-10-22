/* ex3b.c
 * =============================================
 * ==== Written by: Eliachar Feig. ID: 302278338 ===
 * =============================================
 * This program creates 4 pipes for communicates from parent process to sons
 * and builds array of 50000 and insert to it randomally from 0 to 200000.
 * Proceess will make born 3 children, each child will run in an infinite
 * loop and will send to father through the parent pipe randomly a number from
 *  0 to 200000,and send its ID and its pid process the father get that in an
 *  array. it will search the number in the array it first build and through
 *  the accurate pipe (each child with it own pipe) if found or not.
 *  The child will calculate how many numbers he sent and how many caused a
 *  delation in the father array.
 *  When the array is empty the father exit loop and send to children a signal
 *  (SIGTERM) which when they cath it will print their data calculation and
 *  exit process. Father will terminate then the program
 *
 * No input
 * Output : Father: how many numbers got from children, how many he deleted and
 * how much time took for do the job
 * Child: how many number send and how many caused a delete
 */
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
//-----------------------------------------
#define PAPA_READ      pipe_fd[0][0]
#define TO_PAPA        pipe_fd[0][1]

#define	CHILD_READ1    pipe_fd[1][0]
#define	TO_CHILD1      pipe_fd[1][1]

#define CHILD_READ2	   pipe_fd[2][0]
#define TO_CHILD2      pipe_fd[2][1]

#define	CHILD_READ3    pipe_fd[3][0]
#define	TO_CHILD3      pipe_fd[3][1]

#define      MAX_ARR   50000
#define      MAX_RAND  200000
#define      ROW       4
#define      COL       2
#define      ID1       11
#define      ID2       22
#define      ID3       33
#define      TRUE      1
#define      FALSE     0
#define      END       -1
//-------------------PROTOTYPES ------------------
void do_father(int pipe_fd[ROW][COL], int arr[]);
void do_child1(int pipe_fd[ROW][COL], int arr[]);
void do_child2(int pipe_fd[ROW][COL], int arr[]);
void do_child3(int pipe_fd[ROW][COL], int arr[]);
int  delete_from_arr(int arr[], int max_arr, int num_to_del);
void quickSort(int arr[], int low, int high);
int partition (int arr[], int low, int high);
void swap(int* i, int* j);
bool bin_search(int arr[], int num);
void sig_handler1(int stat);
void sig_handler2(int stat);
void sig_handler3(int stat);
void exit_error();
//-------------------------------------
int send_num1 = 0, father_delete1 = 0;
int send_num2 = 0, father_delete2 = 0;
int send_num3 = 0, father_delete3 = 0;
//-------------------------------------
int main()
{
	pid_t pid;
	int pipe_fd[4][2];
	if (pipe(pipe_fd[0]) == END  || pipe(pipe_fd[1]) == END
			||pipe(pipe_fd[2]) == END  || pipe(pipe_fd[3]) == END )
		exit_error();
	int arr[MAX_ARR];
	for (int i = 0; i < MAX_ARR; i++) // build array, max = 50000
		arr[i] = rand()% MAX_RAND;

	for (int child_num = ID1 ; child_num < ID3+1; child_num += ID1) //make 3 child
	{
		pid = fork();
		if (pid < 0)
			exit_error();
		if (pid == 0) // if son
		{
			switch(child_num)
			{
			case ID1: // send to child 1
				do_child1(pipe_fd, arr);  break;
			case ID2: // send to child 2
				do_child2(pipe_fd, arr); break;
			case ID3: // send to child 3
				do_child3(pipe_fd, arr); break;
			}
		}
	}
	//if (pid > 0)
	do_father(pipe_fd, arr);
	return EXIT_SUCCESS;
}
//------------------------------------------------------
void do_father(int pipe_fd[ROW][COL],int arr[])
{
	close(TO_PAPA);close (CHILD_READ1);close(CHILD_READ2);close(CHILD_READ1);
	int array[MAX_ARR], num_of_father_get = 0, num_of_del = 0, found = FALSE;
	quickSort(arr, 0,  MAX_ARR-1);//sort array
	int max_arr = MAX_ARR;
	pid_t pid1, pid2, pid3;
	time_t time1 = time(NULL);
	while(max_arr != 0)
	{
		if (read (PAPA_READ, array, sizeof(array)) <= 0) // [0][0] // read child
			perror("cannot read...." );
		num_of_father_get++;//set

		if (bin_search(arr, array[0]) && !found )
		{
			max_arr = delete_from_arr(arr,max_arr, array[0]);
			num_of_del++; // set
			found = TRUE;
		}
		switch(array[1])
		{
		case ID1: // send to child 1
			if (write (TO_CHILD1, &found, sizeof(int)) <= 0) // [1][1]
				perror("cannot write...\n" );
			pid1 = array[2];
			break;
		case ID2: // send to child 2
			if (write (TO_CHILD2, &found, sizeof(int)) <= 0) // [2][1]
				perror("cannot write...\n" );
			pid2 = array[2];
			break;
		case ID3: // send to child 3
			if (write (TO_CHILD3, &found, sizeof(int)) <= 0) // [3][1]
				perror("cannot write...\n" );
			pid3 = array[2];
			break;
		}
		found = FALSE;
	}
	time_t time2 = time(NULL);
	time_t time_took = time2 - time1;
	printf("father Got %d, num_delete %d, time took %lf \n",
			num_of_father_get, num_of_del, (double)time_took );
	kill(pid1 ,SIGTERM);
	kill(pid2, SIGTERM);
	kill(pid3, SIGTERM);
	close(PAPA_READ);
	exit( EXIT_SUCCESS);
}
//----------------------------------------------
void do_child1(int pipe_fd[ROW][COL], int arr[])
{
	signal(SIGTERM, sig_handler1);
	close(TO_CHILD1);
	int array[4],papa_response;
	while(TRUE)// TRUE
	{
		int num = rand()% MAX_RAND ;
		array[0] = num;
		array[1] = ID1;
		array[2] = getpid();
		if (write (TO_PAPA ,array,  3 * sizeof(int)) > 0) // [0][1]
			send_num1++;
		if (read (CHILD_READ1, &papa_response, sizeof(int)) > 0)// [1][0]
		{
			if (papa_response == TRUE)
				father_delete1++;
		}
	}
	close(CHILD_READ1);
	exit(EXIT_SUCCESS);
}
//----------------------------------------------
void do_child2(int pipe_fd[ROW][COL], int arr[])
{
	signal(SIGTERM, sig_handler2);
	close(TO_CHILD2);
	int array[4], papa_response;
	while(TRUE)
	{
		int num = rand()% MAX_RAND ;
		array[0] = num;
		array[1] = ID2;
		array[2] = getpid();
		if (write (TO_PAPA ,array,  3 * sizeof(int)) > 0) // [0][1]
			send_num2++;

		if (read (CHILD_READ2, &papa_response, sizeof(int)) > 0)// [2][0]
		{
			if (papa_response == TRUE)
				father_delete2++;
		}
	}
	close(CHILD_READ2);
	exit(EXIT_SUCCESS);
}
//----------------------------------------------
void do_child3(int pipe_fd[ROW][COL], int arr[])
{
	signal(SIGTERM, sig_handler3);
	close(TO_CHILD3);
	int array[4], papa_response;
	while(TRUE)
	{
		int num = rand()% MAX_RAND ;
		array[0] = num;
		array[1] = ID3;
		array[2] = getpid();
		if (write (TO_PAPA ,array,  3 * sizeof(int)) > 0 ) // [0][1]
			send_num3++;

		if (read (CHILD_READ3, &papa_response, sizeof(int)) > 0)// [3][0]
		{
			if (papa_response == TRUE)
				father_delete3++;
		}
	}
	close(CHILD_READ3);
	exit(EXIT_SUCCESS);
}
//------------------------------
int delete_from_arr(int arr[], int max_arr, int num_to_del)
{
	int i;
	for (i = 0; i < max_arr; i++)
		if (arr[i] == num_to_del)
			break;
	if (i <= max_arr)
	{
		max_arr--;
		for (int j = i; j < max_arr; j++)
			arr[j] = arr[j+1];
	}
	//printf("\n array: \n");
	//for (i = 0; i < max_arr; i++)
	//	printf("%d \n", arr[i]);
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
	int pivot = arr[high];
	int i = (low - 1);
	for (int j = low; j <= high - 1; j++)
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
//-----------------------------------------------
void sig_handler1(int stat)
{
	signal(SIGTERM, sig_handler1);
	printf("ch1 : %d %d\n", send_num1, father_delete1);
	exit(EXIT_SUCCESS);
}
//------------------------------------------
void sig_handler2(int stat)
{
	signal(SIGTERM, sig_handler2);
	printf("ch2: %d %d\n", send_num2, father_delete2);
	exit(EXIT_SUCCESS);
}
//------------------------------------------
void sig_handler3(int stat)
{
	signal(SIGTERM, sig_handler3);
	printf("ch3: %d %d\n", send_num3, father_delete3);
	exit(EXIT_SUCCESS);
}
//---------------------------------------
void exit_error()
{
	fprintf(stderr,"pipe/fork failed\n");
	exit (EXIT_FAILURE);
}





