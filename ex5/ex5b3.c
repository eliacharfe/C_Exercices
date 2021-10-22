#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>

#define   ARR_SIZE   5
#define   ARR_SIZE2  22
#define   LOCK       4
#define   TRUE       1
#define   FALSE      0
#define   END        -1
//---------------------------------------
void run_front_end(int arr[]);
//void run_front_end(int arr[], int arr2[]);
void sig_catch(int status);
void exit_error(char str[]);
//----------------------------------------------------------------------------
int main()
{
	key_t key , key2;
	int shm_id, shm_id2 ;
	if ((key = ftok(".", 'g'))  == END)
		exit_error("ftok failed") ;
	if ((shm_id = shmget(key, ARR_SIZE, 0600)) == END)
		exit_error("shmget failed") ;
	int *shm_ptr = (int*) shmat(shm_id, NULL, 0) ;
	if (shm_ptr == (int*) END)
		exit_error("shmat failed: ") ;

/*	if ((key2 = ftok(".", 'd'))  == END)
			exit_error("ftok2 failed") ;
		if ((shm_id2 = shmget(key2, ARR_SIZE2, 0600)) == END)
			exit_error("shmget2 failed") ;
		int *shm_ptr2 = (int*) shmat(shm_id2, NULL, 0) ;
		if (shm_ptr2 == (int*) END)
			exit_error("shmat2 failed: ") ;*/

		//run_front_end(shm_ptr, shm_ptr2);
	run_front_end(shm_ptr);

	return EXIT_SUCCESS;
}
//---------------------------------------------
//void run_front_end(int arr[], int arr2[])
void run_front_end(int arr[])
{
	int num, num2;
	char c;
	printf("Enter g or d: ");
	while (scanf("%c", &c))
	{
		switch(c)
		{
		case 'g':
			scanf("%d %d", &num, &num2);
			arr[1] = num;
			arr[2] = num2;
			arr[4] = getpid();
			break;
		case 'd':
			//scanf ("%d" , &num);
			//arr2[1] = num;
			//arr2[21] = getpid();
			break;
		}
		kill(arr[0], SIGUSR1);
		//kill(arr[0], SIGUSR2);
		pause();
		printf("gcd is: %d\n", arr[3]);
	}
}
//---------------------------------
void exit_error(char str[])
{
	perror(str);
	exit (EXIT_FAILURE);
}













