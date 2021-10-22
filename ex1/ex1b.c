/* ./ex1b.c
 * Calculator
 *=============================================
 *==== Written by: Eliachar Feig. ID: 302278338 ====
 *=============================================
 * This program is running in a loop with a recursion function and in
 * each iteration ask from user to choose between 3 programs (or exit).
 * After user chooses a program, a child is born while the father (the
 * current process ) waits for his child. The child will ask for data and
 * then will run the accurate program through execvp to the program. When
 * the program terminate it will return to the father wich will do nothing
 * and continue to the next input.
 *
 * input: 1 = pluc_minus.c
 *        2 = sum.c
 *        3 = max.c
 *        0 = terminate program
 * output: result
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
//-------------------------------------------
enum levelMenu_t { Exit = 0, PlusMinus ,  Sum, Max };
#define MAX_STR_LEN 1000
//-------------------PROTOTYPES ------------------
void Menu(const char str[]);
void buildPlusMinusArguments_andSend();
void buildSumArgs_andSend();
void buildMaxArgs_andSend();
char **read_and_alloc();
//------------------------------------------
int main()
{
	char str[MAX_STR_LEN];
	scanf("%s", str);//read the first program to run
	Menu(str); // recursive function

	return EXIT_SUCCESS;
}
//---------------------------------------------------------------
void Menu(const char str[])
{
	pid_t pid;
	int status;
	if (strcmp(str, "plus_minus" ) == 0)
	{
		pid = fork(); // make a child
		if (pid > 0)//if father
			wait(&status);
		else
			buildPlusMinusArguments_andSend();//son
	}
	else if (strcmp(str, "sum" ) == 0)
	{
		pid = fork(); // make a child
		if (pid > 0)//if father
			wait(&status);
		else
			buildSumArgs_andSend();//son
	}
	else if (strcmp(str, "max" ) == 0)
	{
		pid = fork(); // make a child
		if (pid > 0)//if father
			wait(&status);
		else
			buildMaxArgs_andSend();//son
	}

	char next_str[MAX_STR_LEN];
	scanf("%s", next_str);//read
	Menu(next_str);// call recursively
}
//-----------------------------------------------------
void buildPlusMinusArguments_andSend()
{ // will insert data from user and send to plus_minus.c program
	char str1[MAX_STR_LEN];
	char str2[MAX_STR_LEN];
	scanf("%s", str1);//read
	scanf("%s", str2);//read
	getchar();
	char ch[MAX_STR_LEN];
	fgets(ch, 512, stdin);//read

	char *args[] = { "./plus_minus", str1 , str2, ch, NULL};//insert

	if (execvp("./plus_minus", args) != 0)// go exec
	{//if failed
		perror("execvp plus_minus failed...");
	}
}
//-----------------------------------------------------------
void buildSumArgs_andSend()
{// will insert data from user and send to sum.c program
	getchar();
	size_t size = 0;
	char *str;
	getline(&str, &size, stdin);
	char *arg[] = { "./sum", str , NULL};// insert
	if (execvp("./sum", arg) != 0)
	{//if failed
		perror("execvp sum failed...");
	}
}
//----------------------------------------------------
void buildMaxArgs_andSend()
{// will insert data from user and send to max.c program
	char **arr = read_and_alloc();//read data and builed accurate char array
	arr[0] = "./max";//set
	if (execvp("./max", arr) != 0)
	{//if failed
		perror("execvp max failed...\n");
	}
}
//--------------------------------------------------
char **read_and_alloc()
{ // read a string wich should represent integers separated with space and
	//then calculate the number of words then builds a 2D array of characters
	//(char**) dynamecly which the number of row is equal to the number of
	//words. each row will contain 1 string(without space). returns the array
	getchar();
	size_t size = 0;
	char *str;
	getline(&str, &size, stdin);
	int countSpace = 0;

	for (int i = 0; str[i] != '\n'; i++)
	{
		if (str[i] == ' ')
			countSpace++;
	}
	countSpace++;
	char **arr = malloc (countSpace * sizeof(char));
	//dynamic allocation (rows)
	if (arr == NULL)//check
	{ perror("cannot alloc...");
	exit(EXIT_FAILURE);}

	int index_string = 0,row = 1, col = 0;
	arr[row] =  malloc(MAX_STR_LEN* sizeof(char));//alloc columns first row
	if (arr[row] == NULL) //check
	{	perror("cannot alloc...");
	exit(EXIT_FAILURE); }
	for (index_string = 0; str[index_string] != '\n'; index_string++)
	{
		if (str[index_string] != ' ')
		{
			arr[row][col] = tolower(str[index_string]);//copy single char
			col++;//set
		}
		else
		{// if next number (string)
			row++;//set
			arr[row] =  malloc(MAX_STR_LEN *sizeof(char));// alloc new row
			if (arr[row] == NULL)//check
			{	perror("cannot alloc...");
			exit(EXIT_FAILURE); }
			col = 0;//reset
		}
	}
	arr[countSpace+1] = NULL;//put NULL in the num of rows + 1
	return arr;
}

