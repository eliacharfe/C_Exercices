/* ex2a.c
 * =============================================
 * ==== Written by: Eliachar Feig. ID: 302278338 ===
 * =============================================
 * This program make born a child which will cast lots of integers and
 * find the 10 first different prime numbers and if find them before 3 secods
 * alarm that the father give to him to do the job, he blocks the signal
 * SIGTERM from father and prints those primes, after that happens the father
 * prints "good boy" (by a signal handler function when it gets the signal
 * SIGCHILD). If the boy didnt finish within 3 seconds the father kill the
 * son with SIGTERM signal and prints "bad boy" then terminate program
 *
 * No input
 * Output : according to signals
 */
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdbool.h>
//-------------------PROTOTYPES ------------------
void do_father();
void do_child();
bool exist(int arr[], int num, int counter);
bool is_prime(int num);
void sig_handler_good(int sig);
void sig_handler_bad(int sig);
int block_and_print(int arr[]);
//--------------------------------------
#define MAX 10
#define MAX_ALARM 3
pid_t son_pid;
//-----------------------------------------
int main()
{
	pid_t pid = fork();
	son_pid = pid;
	if(pid > 0) //father
		do_father();
	else //son
		do_child();
	return EXIT_SUCCESS;
}
//-------------------------------------
void do_father()
{
	signal(SIGCHLD, sig_handler_good);//if child return
	signal(SIGALRM, sig_handler_bad);//if child not return
	alarm(MAX_ALARM);// 3 sec
	pause();
}
//------------------------------------------
void do_child()
{
	srand (17);// big numbers
	int  arr[MAX], counter = 0;
	while (counter != MAX) // until counter get 10 diff primes
	{
		int num = rand();
		if (is_prime(num) && !exist(arr, num, counter))
			counter++;//if is prime and not in the array, add it and increase
	}
	block_and_print(arr);//child will done it jobs if get here
	exit(EXIT_SUCCESS);
}
//----------------------------------
int block_and_print(int arr[])
{//child will accomplish his task by blocking the signal SIGTERM + print array
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask,SIGTERM);
	sigprocmask(SIGTERM,&mask, NULL);  // block this signal
	for(int i = 0; i < MAX; i++)
		printf("%d ", arr[i]);// print array
	exit(EXIT_SUCCESS);
}
//------------------------
bool exist(int arr[], int num, int counter)
{// check if num (prime) is already inside the array if not return false
	int index;
	for (index = 0;index < counter; index++)
	{
		if (arr[index] == num) // check if the prime is already in array
			return true;
	}
	arr[index] = num;// if is not = add it to the next place
	return false;
}
//---------------------------------------
bool is_prime(int num)
{// check if the number is a prime
	bool isPrime = false;
	for (int i = 2; i <= num / 2; ++i)
	{
		if (num % i == 0)
		{
			isPrime = 1;
			break;
		}
	}
	if (isPrime == 0)
		return true;
	return false;
}
//----------------------------------------
void sig_handler_bad(int son_pid)
{// if take child more then 3 sec
	printf("bad boy... \n");// print
	kill(son_pid == 0 ,SIGTERM); // kill son by sending SIGTERM signal
	exit(EXIT_SUCCESS);
}
//----------------------------------------
void sig_handler_good(int sig)
{// if child finish in time
	printf("\ngood boy... \n"); //print
	exit(EXIT_SUCCESS);
}
