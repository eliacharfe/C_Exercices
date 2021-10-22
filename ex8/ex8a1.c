/* ./ex5b
 * Shared memory: Common Greatest Divisor + Decomposition
 * ========================================================
 * === Written by: Eliachar Feig. id: 302278338 ===
 * =======================================================
 * This program run 2 different process that read and write into a common
 * shared memory.
 * ex5a1: allocates an array of size 1000. in the first cell puts its pid.
 * puts in the tree next cells 0, which will contains pids of 3 process.
 * in the 5th cell will insert 0, which this cell will be a lock to comunicate
 * betwwen the "father" and the "child" process telling each one to the other
 * if still works or not. if the other one works then the curr process sleeps.
 * in the rest of the cell will insert randomally from 0 to 200000. He go to
 * sleep until the children done their job and tell how mush time took.
 * ex5a2: get in arguments vector 1, 2 or 3 which will tell the seed. first
 * thing it put in the array at the cell of the number got in argv[1] the
 * number 1, and wait until the cell 1, 2 and 3 are all = 1, then continue
 * to main loop (waiting for his "brothers". then until the array is empty
 * (between 5 to 1000 cells) cast randomally a number from 0 to 200000 then
 * go sleep until 5th cell is = 0, then lock the cell putting 1 then check
 * if the number is found in the array, if so then delete (puts -1), then puts
 * 0 at the 5th cell (to unlock). when the counter of cells wich are empty
 * reaches the size of the array (minus 5) then exit loop , tell how much
 * send to father and how much caused delete
 *
 * No Input.
 * Output: server: how much time took
 *         child process: how much and how much caused delete
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>

#define   ARR_SIZE   50
#define   MAX_RAND   200000
#define   LOCK       4
#define   TRUE       1
#define   FALSE      0
#define   END        -1
//---------------------------------------
void insert_first_cells(int shm_ptr[]);
void run_server_til_sig(int shm_id, int shm_ptr[]);
void build_array(int shm_ptr[]);
void sig_catch(int status);
void exit_error(char str[]);
//----------------------------------------------------------------------------
int main()
{
	signal (SIGINT , sig_catch);
	key_t key ;
	int shm_id ;
	if ((key = ftok(".", '8')) == END)
		exit_error("ftok failed") ;
	if ((shm_id = shmget(key, ARR_SIZE, IPC_CREAT | IPC_EXCL | 0600)) == END)
		exit_error("shmget failed: ") ;
	int *shm_ptr ;
	if ((shm_ptr = (int*) shmat(shm_id, NULL, 0))  == (int*) END)
		exit_error("shmat failed: ") ;

	insert_first_cells(shm_ptr);
	build_array(shm_ptr);
	run_server_til_sig(shm_id, shm_ptr);

	return( EXIT_SUCCESS ) ;
}
//---------------------------------------------------
void insert_first_cells(int shm_ptr[])
{
	shm_ptr[0] = getpid() ;
	for (int i = 1; i < 4; i++)
		shm_ptr[i] = 0;
}
//----------------------------------------
void run_server_til_sig(int shm_id, int shm_ptr[])
{
	sem_t *mutex = sem_open("/my_mutex", IPC_CREAT, 0644, 1);
	if(mutex == SEM_FAILED)
	{
		perror("father sem open failed");
	}

	time_t time_start = time(NULL);

	sem_wait(mutex);
	pause();
	sem_post(mutex);
	time_t time_end = time(NULL);
	time_t time_took = time_end - time_start;
    printf("%lf\n", (double)time_took) ;
	if (shmctl(shm_id, IPC_RMID, NULL) == END)
		exit_error("shmctl failed\n") ;

	exit(EXIT_SUCCESS) ;
}
//------------------------------------------------
void build_array(int shm_ptr[])
{
	for (int i = 4; i < ARR_SIZE; i++) // build array, max = 50000
		shm_ptr [i] = rand()% MAX_RAND;// 0 -20000
}
//-----------------------------------------------------------
void sig_catch(int status)
{ sleep(1); }
//--------------------------
void exit_error(char str[])
{
	perror(str);
	exit (EXIT_FAILURE);
}

