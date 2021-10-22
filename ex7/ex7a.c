/* ./ex7a
 * Shared memory: Common Greatest Divisor + Decomposition
 * ========================================================
 * === Written by: Eliachar Feig. id: 302278338 ===
 * =======================================================
This program uses threads,
The father cast in an array of 5000 numbers from 0 to 200000
and make born 3 threads which each of them run until the array is empty
the first cell in the array is a lock , 0 if is open , 1 if is close. each child will be
block by the first cell if it is 1 when it become zero it get in and lock by putting
1, then he check if the number he castsed is found in the global array, if so
he delete it

 * No Input.
 * Output: father: how much time took
 *         child (each): how much and how much caused delete
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>

#define NUM_CHILD 3
#define ARR_SIZE  5000
#define MAX_RAND  200000
#define SEED      17
#define LOCK      1
#define UNLOCK    0
#define END      -1
//--------------------------------------
void* my_func(void *) ;
void build_array();
int delete_from_arr(int max_arr, int num_of_del);
void quickSort(int low, int high);
int partition (int low, int high);
void swap(int* i, int* j);
bool bin_search(int num);
void exit_error(char str[]);
//------------------------------------------
int arr[ARR_SIZE];
int max_arr = ARR_SIZE-1;
//-------------------------------------------------------
int main()
{
	pthread_t thread_id[NUM_CHILD] ;
	int ids[NUM_CHILD] = {0, 1, 2} ;
	int i ;
	int status ;

	build_array();
	quickSort(1, ARR_SIZE-1);

	time_t time_start = time(NULL);
	for (i = 0; i < NUM_CHILD ; i++)
	{
		status = pthread_create(&(thread_id[i]), NULL, my_func, (void *) &(ids[i])) ;
		if (status != 0)
			exit_error("pthread_create failed in main") ;
	}
	for (i= 0; i< NUM_CHILD; i++)
		status = pthread_join(thread_id[i],NULL) ;
	time_t time_end = time(NULL);
	time_t time = time_end - time_start;

	printf("time took: %lf\n", (double)time);
	puts("\nFather end") ;
	pthread_exit(NULL) ;
	return(EXIT_SUCCESS) ;
}
//------------------------------------------------
void build_array()
{
	srand(SEED);
	arr[0] = 0;
	for (int i = 1; i < ARR_SIZE; i++) // build array, max = 50000
		arr[i] = rand()% MAX_RAND;// 0 -20000
}
//-------------------------------------------------------
void* my_func(void * args)
{
	int id = *((int *) args) ;
	printf("child %d start\n", id);
	int  num_child_rand = 0, num_of_del = 0;
	while (max_arr != 0)
	{
		int rand_num = rand()% MAX_RAND;
		while (arr[0] == LOCK)  // arr[4] == 1 // is locked
		{	;}
		arr[0] = LOCK /*1*/;// when open (0) --> so close (1)
		num_child_rand++;
		if (bin_search(rand_num))
		{
			max_arr = delete_from_arr( max_arr, rand_num);
			num_of_del++;
		}
		arr[0] = UNLOCK/*0*/;// arr[4] = 0
	}
	printf("num child rand: %d num_of_del: %d \n", num_child_rand, num_of_del);
	pthread_exit(NULL) ;
}
//------------------------------------------------------
int delete_from_arr(int max_arr, int num_to_del)
{
	int i, j;
	for (i = 1; i < max_arr; i++)
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
bool bin_search(int num)
{
	int first = 0;
	int last = ARR_SIZE - 1;
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
void quickSort(int low, int high)
{
	if (low < high)
	{
		int pivot = partition(low, high);
		quickSort(low, pivot - 1);
		quickSort(pivot + 1, high);
	}
}
//------------------------------------------------
int partition (int low, int high)
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
//-------------------------------------------
void exit_error(char str[])
{
	perror(str);
	exit (EXIT_FAILURE);
}
//------------------------------
