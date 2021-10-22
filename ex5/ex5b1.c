#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>

#define   ARR_SIZE   5
#define   LOCK       4
#define   TRUE       1
#define   FALSE      0
#define   END        -1
//---------------------------------------
void run_greatest_common_divisor(int arr[]);
int max_div(int num1, int num2);
void sig_catch(int status);
void exit_error(char str[]);
//----------------------------------------------------------------------------
int main()
{

	signal(SIGUSR1, sig_catch);
	key_t key ;
	int shm_id ;

	if ((key = ftok(".", 'g')) == END)
		exit_error("ftok failed") ;
	if ((shm_id = shmget(key, ARR_SIZE, IPC_CREAT | IPC_EXCL | 0600)) == END)
		exit_error("shmget failed") ;
	int *shm_ptr ;
	if ((shm_ptr = (int*) shmat(shm_id, NULL, 0))  == (int*) END)
		exit_error("shmat failed: ") ;

	run_greatest_common_divisor(shm_ptr);

	return EXIT_SUCCESS;
}
//---------------------------------------------------------
void run_greatest_common_divisor(int arr[])
{
	arr[0] = getpid();
	while (TRUE)
	{
		for (int i = 0; i < 5; i++)
			printf ("arr[%d] = %d\n", i, arr[i]);
		printf("fath before pause\n");
		pause();
		printf("fath after pause\n");
		arr[3] = max_div(arr[1], arr[2]);
        kill (arr[4], SIGUSR1);
	}
}
//--------------------------------
int max_div(int num1, int num2)
{
	int gcd;
	for(int i = 1; i <= num1 && i <= num2; ++i)
	{
		if(num1 % i == 0 && num2 % i == 0)
			gcd = i;
	}
	return gcd;
}
//--------------------------------------
void sig_catch(int status)
{
	;
}
//-------------------------------------
void exit_error(char str[])
{
	perror(str);
	exit (EXIT_FAILURE);
}

