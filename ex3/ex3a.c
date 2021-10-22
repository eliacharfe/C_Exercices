/* ex3a.c
 * =============================================
 * ==== Written by: Eliachar Feig. ID: 302278338 ===
 * =============================================
 * This program creates 4 pipes for communicates from parent process to sons
 * proccess. It builds an array of random numbers from 0 to 200000 of size of
 * 50000, then make born 2 children then in a loop until 1000 and in each
 * iteration cast a number from 0 to 200000 and send it to both children the
 * number.  the first child will search linary the number in the array then
 * send how much time it takes to search. the second child will do the same
 * thing but will search binary (first thing it will sort the array before the
 * loop). The children close the standard output then do dup to their pipe
 * and just do prints the result which the father get in each pipe (according
 * to the accurate pipe of child). The father et the result of the current time
 * took for each child and sum it, and save the max time and the minimum time.
 * when the loop get 1000 it will exit loop and send -1 to the child wich tell
 * the to terminate their process and the father prints the resultd from each
 * child the avarge time took the max time and the min time. The terminate prog
 *
 * No input
 * Output : avarage time, max time, mun time of both children
 */
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
//-------------------PROTOTYPES ------------------
void do_father(int pipe_fd[], int pipe_fd2[],
		int pipe_fd3[], int pipe_fd4[], int arr[]);
void do_child1(int pipe_fd[2], int pipe_fd2[2], int arr[]);
time_t search_linary(int arr[], int num);
void do_child2(int pipe_fd3[], int pipe_fd4[], int arr[]);
void quickSort(int arr[], int low, int high);
int partition (int arr[], int low, int high);
void swap(int* i, int* j);
void bin_search(int arr[], int num);
void exit_error();
//-----------------------------------------
#define	PARENT_READ1   pipe_fd2[0]
#define	CHILD_WRITE1   pipe_fd2[1]

#define CHILD_READ1	   pipe_fd[0]
#define PARENT_WRITE1  pipe_fd[1]

#define	PARENT_READ2   pipe_fd4[0]
#define	CHILD_WRITE2   pipe_fd4[1]

#define CHILD_READ2	   pipe_fd3[0]
#define PARENT_WRITE2  pipe_fd3[1]

#define     MAX_ARR    50000
#define     MAX_RAND   200000
#define     MAX_READ   1000
#define     ID1        1
#define     ID2        2
#define     END        -1
//-------------------------------------
int main()
{
	pid_t pid;
	int pipe_fd[2], pipe_fd2[2], pipe_fd3[2], pipe_fd4[2];
	if (pipe(pipe_fd) == END || pipe(pipe_fd2) == END
			|| pipe(pipe_fd3) == END || pipe(pipe_fd4) == END)
		exit_error();

	int arr[MAX_ARR];
	for (int i = 0; i < MAX_ARR; i++) // until 50000
		arr[i] = rand()% MAX_RAND; // from 0 to 200000

	for (int child_num = ID1 ; child_num <= ID2; child_num++)
	{
		pid = fork(); // make born 2 children
		if (pid < 0)
			exit_error();
		if (pid == 0)
		{
			switch (child_num)
			{
			case ID1: // child 1
				do_child1(pipe_fd, pipe_fd2, arr) ; break;
			case ID2: // child 2
				do_child2(pipe_fd3, pipe_fd4, arr); break;
			}
		}
	}
	if (pid > 0)
		do_father(pipe_fd, pipe_fd2, pipe_fd3, pipe_fd4, arr);// father
	return EXIT_SUCCESS;
}
//------------------------------------------------------
void do_father(int pipe_fd[], int pipe_fd2[],
		int pipe_fd3[], int pipe_fd4[],int arr[])
{
	close(CHILD_READ1); close (CHILD_READ2);
	close(CHILD_WRITE1); close(CHILD_WRITE2);
	int counter = 0;
	double max = 0, min = 0, sum = 0, max2 = 0, min2 = 0, sum2 = 0;
	char str[MAX_READ], str2[MAX_READ];
	srand(17);
	while(counter != MAX_READ)
	{
		int num = rand()% MAX_RAND;
		if (write(PARENT_WRITE1,&num,sizeof(int)) <= 0)// write to child 1
			perror("cannot write...\n");
		if(write(PARENT_WRITE2,&num,sizeof(int)) <= 0)// write to child 2
			perror("cannot write2...\n");

		if(read(PARENT_READ1, str, sizeof(str)) <= 0) // read from child 1
			perror("cannot read...\n");
		num = atoi(str);
		sum += num; // child 1
		if (num >= max)
			max = num;
		if (num <= min)
			min = num;

		if (read(PARENT_READ2, str2, sizeof(str2)) <= 0)// read from child 2
			perror("cannot read...\n");
		double num2 = atoi(str2);
		sum2 += num2; // child 2
		if (num2 >= max2)
			max2 = num2;
		if (num2 <= min2)
			min2 = num2;

		counter++;
	}
	double avarage = sum / MAX_READ; double avarage2 = sum2 / MAX_READ;
	printf("%lf %lf %lf ",avarage, max, min);
	printf("%lf %lf %lf\n", avarage2, max2, min2);
	int END_PROG = END;// -1
	write(PARENT_WRITE1, &END_PROG ,sizeof(int));//send -1 to tell child 1 exit
	write(PARENT_WRITE2, &END_PROG ,sizeof(int));//send -1 to tell child 2 exit
	close(PARENT_WRITE1); close(PARENT_READ1);
	close(PARENT_WRITE2); close(PARENT_READ2);
}
//----------------------------------------------
void do_child1(int pipe_fd[2], int pipe_fd2[2], int arr[])
{
	close(PARENT_WRITE1);
	close(PARENT_READ1);
	dup2(CHILD_WRITE1, STDOUT_FILENO); // close standard output
	int counter = 0, num;
	while(counter != MAX_READ)
	{
		if (read(CHILD_READ1,&num,sizeof(int)) <= 0)
			perror("cannot read...\n");
		if (num == END)
			exit(EXIT_SUCCESS);
		time_t time1 = time(NULL);
		time_t time2 = search_linary(arr, num);// do search
		time_t curr_time = time2 - time1;
		printf("%lf", (double)curr_time);//to "father"print how mush time took
		fflush(stdout);
		counter++;
	}
	close(CHILD_READ1);
	close(CHILD_WRITE1);
	exit (EXIT_SUCCESS);
}
//--------------------------------------------------
void do_child2(int pipe_fd3[], int pipe_fd4[], int arr[])
{
	int num ;
	close(PARENT_WRITE2);
	close(PARENT_READ2);
	dup2(CHILD_WRITE2, STDOUT_FILENO); // make dup
	int counter = 0;

	quickSort(arr, 0, MAX_ARR-1);// sorting the array

	while(counter != MAX_READ)
	{
		if (read(CHILD_READ2, &num, sizeof(int)) <= 0) // read num
			perror("cannot read\n");
		if (num == END)
			exit(EXIT_SUCCESS);
		time_t time1 = time(NULL);
		bin_search(arr, num);// do search
		time_t time2 = time(NULL);
		time_t curr_time = time2 - time1;
		printf("%lf", (double)curr_time);//print to "father how mush time took
		fflush(stdout);
		counter++;
	}
	close(CHILD_READ2);
	close(CHILD_WRITE2);
	exit (EXIT_SUCCESS);
}
//------------------------------
time_t search_linary(int arr[], int numm)
{
	for(int i = 0; i < MAX_ARR; i++)
		if (arr[i] == numm)
		{
			time_t time2 = time(NULL);
			return time2;
		}
	time_t time2 = time(NULL);
	return time2;
}
//---------------------------------------------------------------------------
void bin_search(int arr[], int num)
{
	int first = 0;
	int last = MAX_ARR - 1;
	int middle = (first + last)/2;
	while (first <= last)
	{
		if (arr[middle] < num)
			first = middle + 1;
		else if (arr[middle] == num) {
			return;
		}
		else
			last = middle - 1;
		middle = (first + last)/2;
	}
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
//---------------------------------------
void exit_error()
{
	fprintf(stderr,"pipe/fork failed\n");
	exit (EXIT_FAILURE);
}

