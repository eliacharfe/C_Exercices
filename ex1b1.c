/* ./ex1b1
 * Tower Of Hanoi 
 *=============================================
 *==== Written by: Eliachar Feig. ID: 302278338 ====
 *=============================================
 * This program is getting a number of child from it father and "NUL"
 * file.Then open "NUL" file for reading. each child is using algorithm
 * tower of hanoi recursively, starting with the first child at 10 coins ,
 * and end up with the 5th child doing it at 30 coins, and calculate how
 * many time takes for doing the task for each child and print result
 *
 *  Input: no input
 *  Output: num of coins and the time needed to finish task
 */
#pragma warning(disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <process.h>
 //--------------------------------------------
time_t do_child_i(int, FILE*);
void hanoiTowers(int, char, char, char, FILE*);
//---------------------------------------------
FILE* file = NULL; // open globaly
//----------------------------------------
int main(int argc, char*argv[])
{
	file = fopen(argv[2], "w");// open "NUL" file
	if (file == NULL)
	{//if failed
		perror("Cannot open file...");
		exit(EXIT_SUCCESS);
	}
	printf("%d\n", (int)(do_child_i(atoi(argv[1]), file))); //result
	fclose(file);

	return EXIT_SUCCESS;
}
//---------------------------------
time_t do_child_i(int num_of_child, FILE*file)
{// send for doing the task and calculates the time needed for accomplish task
	time_t time1 = time(NULL);
	int num_of_coins = num_of_child * 5 + 10;//calculate number of coins
	printf("%d = ", num_of_coins);//print
	hanoiTowers(num_of_coins, 'A', 'C', 'B', file);// calling with number of coins
	time_t time2 = time(NULL);
	return (time2 - time1);// returns how much time
}
//--------------------------------------------------
void hanoiTowers(int num_of_coins, char from, char to, char aux, FILE* file)
{
	int num_of_child = (num_of_coins - 10) / 5;
	if (num_of_coins == 1)
		return;

	hanoiTowers(num_of_coins - 1, from, aux, to, file);//recusively
	hanoiTowers(num_of_coins - 1, aux, to, from, file);//recusively
}

//---------------------------------------------------------------



