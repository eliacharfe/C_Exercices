/* ./ex7b
 * Threads: Common Greatest Divisor + Decomposition
 * ========================================================
 * === Written by: Eliachar Feig. id: 302278338 ===
 * =======================================================
 * This program uses threads, the father make born 3 threads
 * and save their id's in a global array (of 3)
 * 3rd child: reads from the user in infinite loop a char:
 *  if the char is 'g':
 * then ask for 2 integers and send it through an array (specific to him)
 *  in the first 2 cells (the array is globaly) then send signal SIGUSR1
 *   to 1st child and go to sleep until get a signal. the 1st child
 * calculates the greatest common divisor (gcd) and return the answer in
 * the next cell in the array. and send SIGUSR1 to 3rd child which will print
 * the answer which he find in 3rd cell.
 * if the char is 'd':
 * then ask for an integer from the user and send it to an (other array) that
 * 2nd child which decompose the number to primes numbers and return the
 * answer in the array between cell 0 to 10. then 3rd child go sleep until
 * get signal. the 2nd child get the number to decompose and put the answer
 * in the array and send signal SIGUSR2 which will wake the 3rd child which
 * print the answer..

 * All program terminate when get signal SIGINT
 *
 * Input: in infinite loop: scan a char:
 *  if 'g': then get 2 integers
 *  if 'd': then get 1 integers
 *
 *  Output: according to the char:
 *  if 'g': gcd
 *  if 'd': decomposition of primes
 *  */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>

#define NUM_CHILD 	  3
#define SEED     	 17
#define ID1   	      0
#define ID2     	  1
#define ID3     	  2
#define TRUE    	  1
#define GCD_ARR_SIZE  3
#define DEC_ARR_SIZE 10
#define END    	     -1
//--------------------------------------
void* call_threads_functions(void *) ;
void run_greatest_common_divisor();
int max_div(int num1, int num2);
void run_decomposition();
void decompose(int num);
void run_front_end();
void sig_catch(int status);
void sig_terminate(int status);
void  exit_error(char str[]);
//---------------------------------
pthread_t thread_id[NUM_CHILD] ;
int arr_gcd[GCD_ARR_SIZE];
int arr_decompose[DEC_ARR_SIZE] = {0};
//-------------------------------------------------------
int main()
{
	signal (SIGINT, sig_terminate);
	int ids[NUM_CHILD] = {0, 1, 2};
	int status;
	for (int i = 0; i < NUM_CHILD ; i++)
	{
		status = pthread_create(&(thread_id[i]), NULL, call_threads_functions,
				(void *) &(ids[i])) ;
		if (status != 0)
			exit_error("pthread_create failed in main") ;
	}
	for (int i= 0; i< NUM_CHILD; i++)
		status = pthread_join(thread_id[i], NULL) ;

	pthread_exit(NULL) ;
	return EXIT_SUCCESS;
}
//---------------------------------------------
void* call_threads_functions(void * args)
{
	int id = *((int *) args) ;
	printf("child %d start, id thread: %d\n", id, (int)pthread_self());

	switch(id)
	{
	case ID1:
		sleep(1);
		run_greatest_common_divisor(); break;
	case ID2:
		sleep(1);
		run_decomposition(); break;
	case ID3:
		run_front_end(); break;
	}
	pthread_exit(NULL) ;
}
//---------------------------------------------------------
void run_greatest_common_divisor()
{
	signal (SIGINT, sig_terminate);
	signal(SIGUSR1, sig_catch);
	while (TRUE)
	{
		pause();
		arr_gcd[2] = max_div(arr_gcd[0], arr_gcd[1]);
		pthread_kill (thread_id[ID3], SIGUSR1);
	}
}
//--------------------------------------------
void run_decomposition()
{
	signal (SIGINT, sig_terminate);
	signal(SIGUSR2, sig_catch);
	while (TRUE)
	{
		pause();
		decompose(arr_decompose[0]);
		pthread_kill (thread_id[ID3], SIGUSR2);
	}
}
//------------------------------------------
void run_front_end()
{
	signal (SIGINT, sig_terminate);
	signal(SIGUSR1, sig_catch);
	signal(SIGUSR2, sig_catch);
	int num, num2;
	char c;
	printf("Enter g or d: ");
	while (scanf("%c", &c))
	{
		switch(c)
		{
		case 'g':
			scanf("%d %d", &num, &num2);
			arr_gcd[0] = num;
			arr_gcd[1] = num2;
			pthread_kill(thread_id[ID1], SIGUSR1);
			pause();
			printf("%d\n", arr_gcd[2]);
			break;
		case 'd':
			scanf ("%d" , &num);
			arr_decompose[0] = num;
			pthread_kill(thread_id[ID2], SIGUSR2);
			pause();
			for(int i = 0 ; arr_decompose[i] != 0 ; i++)
			{
				printf("%d ", arr_decompose[i]);
				arr_decompose[i] = 0; // reset to zero
			}
            printf("\n");
			break;
		}
	}
}
//---------------------------------------------
//-------------------------------------
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
//----------------------------------------------------
void decompose(int num)
{
	int counter = 0;
	while(num % 2 == 0)
	{
		arr_decompose[counter] = 2;
		counter++;
		num = num/2;
	}
	for (int i = 3; i <= (num/2); i+= 2)
	{
		while (num % i == 0)
		{
			arr_decompose[counter] = i;
			num = num/i;
			counter++;
		}
	}
	if(num > 2)
	{
		arr_decompose[counter] = num;
	}
}
//--------------------------------------
void sig_catch(int status)
{ ; }
//-----------------------------------------
void sig_terminate(int status)
{
	printf(": terminate\n");
	exit(EXIT_SUCCESS);
}
//--------------------------------------------
void exit_error(char str[])
{
	perror(str);
	exit (EXIT_FAILURE);
}








