/* ./ex1a
 * Tower Of Hanoi - Fork
 *=============================================
 *==== Written by: Eliachar Feig. ID: 302278338 ====
 *=============================================
 * This program is creating 5 child process, each child is using algorithm
 * tower of hanoi recursively, starting with the first child at 10 coins ,
 *  and end up with the 5th chld doing it at 30 coins.
 *
 *  Input: no input
 *  Output: num of coins and the time needed to finish task
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_CHILD 5
#define MAX_FILE_NAME 20
//--------------------------------------------
time_t do_child_i(int , FILE * file);
void hanoiTowers(int , char , char , char, FILE * file);
//---------------------------------------------
int main()
{
	for (int num_of_child  = 0; num_of_child  < MAX_CHILD; num_of_child ++)
	{
		pid_t pid = fork();
		if (pid)
		{
			FILE * file = NULL;
			file = fopen("/dev/null" , "w");
			printf ("%d\n", (int)(do_child_i(num_of_child , file)));
			fclose(file);
			return EXIT_SUCCESS;
		}
	}


	return EXIT_SUCCESS;
}
//---------------------------------
time_t do_child_i(int num_of_child , FILE * file)
{
	time_t time1 = time(NULL);
	int num_of_coins = num_of_child  * 5 + 10;
    printf("%d = ", num_of_coins);
    hanoiTowers(num_of_coins, 'A', 'C', 'B', file);

    time_t time2 = time(NULL);
    time_t time = time2 - time1;
    return time;
}
//--------------------------------------------------
void hanoiTowers(int num_of_coins, char from, char to, char aux, FILE * file)
{
	//int num_of_child = (num_of_coins - 10) / 5;
	//fprintf(file, "child : %d\n  num of coins are %d\n" ,num_of_child, num_of_coins);
    if (num_of_coins == 1)
        return;

    hanoiTowers(num_of_coins - 1, from, aux, to, file);
    fprintf(file ,"\n Move coin %d from %c to %c", num_of_coins, from, to);
    hanoiTowers(num_of_coins - 1, aux, to, from, file);
}

//---------------------------------------------------------------




