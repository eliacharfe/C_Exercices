#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>

#define   ARR_SIZE   50
#define   MAX_RAND   200000
#define   TRUE       1
#define   ONE        1
#define   LOCK_CELL  4
#define   LOCK       1
#define   UNLOCK     0
#define   FIVE       5
#define   END        -1
//---------------------------------------
int delete_from_arr(int arr[], int num_to_del ,int *num_of_del);
void exit_error(char str[]);
//----------------------------------------------
int main(int argc, char *argv[])
{
	key_t key ;
	int shm_id ;
	if ((key = ftok(".", '8'))  == END)
		exit_error("ftok failed") ;
	if ((shm_id = shmget(key, ARR_SIZE, 0600)) == END)
		exit_error("shmget failed") ;
	int *shm_ptr = (int*) shmat(shm_id, NULL, 0) ;
	if (shm_ptr == (int*) END)
		exit_error("shmat failed: ") ;

	shm_ptr[atoi(argv[ONE])] = ONE;

	sem_t *mutex = sem_open("/my_mutex", 0);
	if(mutex == SEM_FAILED)
		perror("child sem open failed");

	int  num_client_rand = 0, num_of_del = 0;
	srand(atoi(argv[1]));
	//shm_ptr[LOCK_CELL] = UNLOCK; // 0
	int counter = FIVE;
	while (TRUE)
	{
		int rand_num = rand()% MAX_RAND;
		//while (shm_ptr[LOCK_CELL] == LOCK)  // arr[4] == 1 // is locked
		//	sleep(1);

		sem_wait(mutex);
		//shm_ptr[LOCK_CELL] = LOCK /*1*/;// when open (0) --> so close (1)

		num_client_rand++;

	  counter += delete_from_arr(shm_ptr, rand_num, &num_of_del);
		if (counter == ARR_SIZE)
		{ // all array is empty
			num_of_del++;
			break;
		}
		sem_post(mutex);
		//shm_ptr[LOCK_CELL] = UNLOCK/*0*/;// arr[4] = 0
	}
	kill (shm_ptr[0], SIGINT); // send to server
	printf("%d %d \n", num_client_rand, num_of_del);
	return( EXIT_SUCCESS ) ;
}
//------------------------------------------------------
int delete_from_arr(int arr[], int num_to_del, int *num_of_del)
{
	int check = 1;
	int i;
	for (i = 5; i < ARR_SIZE; i++)
		if (arr[i] == num_to_del)
		{ // looking for the num
			arr[i] = END;// putting -1 to tell is empty
			(*num_of_del)++;
			break;
		}
	if (i <= ARR_SIZE)
		for (int i = 5; i < ARR_SIZE; i++)
			if (arr[i] != END)
				check = 0;// if at least one cell is not empty

	return check;// if check is = 1 then all array is empty
}
//-----------------------------------------------------------
void exit_error(char str[])
{
	perror(str);
	exit (EXIT_FAILURE);
}
//------------------------------


