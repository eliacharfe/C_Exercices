/* ex1b2
==============================================
==== Written by : Eliachar Feig. ID: 302278338 ====
==============================================
This program is running in a loop and make 5 children and send the number 
of child to ex1b1.c (ex1b1.exe) file wich calculates for each child how many time takes
to do the task:
1st child gets 10 coins
2nd child gets 15 coins
3rd child gets 20 coins
4th child gets 25 coins
5th child gets 30 coins

No input
Output: number of coins, = , time to do the job
*/
#pragma warning(disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <process.h>
//---------------------------------------------
#define MAX_CHILD 5
#define DECIMAL 10 // for itoa function
//-------------------------------------------------
int main()
{
	for (int num_of_child = 0; num_of_child < MAX_CHILD; num_of_child++)
	{	
		char str[2]; 
	    itoa(num_of_child, str, DECIMAL);//converting num of child to string
		if (_spawnlp(_P_WAIT, "ex1b1.exe", "ex1b1.exe", str, "NUL", NULL) == -1)
		{// if failed spawn
			perror("Cannot spawn...\n");
			exit(EXIT_FAILURE);
		}
	}
	return EXIT_SUCCESS;
}
//----------------------------------------------------------------------