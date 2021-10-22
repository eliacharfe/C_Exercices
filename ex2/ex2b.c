/* ex2b.c
 * =============================================
 * ==== Written by: Eliachar Feig. ID: 302278338 ===
 * =============================================
 * This program make born a child then both father and son will run in an
 * infinit loop in which in every iteration cst a lots between 1 and 10 if
 * the number is smaller then 3 the process will print that he ends (with
 * telling his pid). if not it will send a signal to the other process, if
 * the other process didnt gets the signal within 5 seconds it will prints
 * that he was left alone and terminate program. if the process gets the sig-
 * nal within 5 sec, it will print its pid and say that he have a partner.

 * No input
 * Output: according to the signal each process gets
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
void sig_handler(int sig);
void sig_quit(int sig);
//--------------------------------------
#define MAX 10
#define MAX_ALARM 5
//-----------------------------------------
pid_t son_pid;
pid_t father_pid;
//------------------------------------------
int main()
{
	pid_t pid = fork();
	son_pid = father_pid = pid;
	if(pid > 0) //father
		do_father();
	else //son
		do_child();
	return EXIT_SUCCESS;
}
//------------------------------------
void do_father()
{
	while(true)
	{
		int num = rand() % 10;// between 1 and 10
		if (num < 3)
		{
			printf("process %d ends\n", getpid());
			exit(EXIT_SUCCESS); // end process
		}
		else
		{
			kill(son_pid, SIGCONT);//send signal to child
			signal(SIGCONT, sig_handler);// get signal from child
			signal(SIGALRM, sig_quit);// if didnt get a signal in 5 sec
			alarm(MAX_ALARM);
			pause();
		}
	}
	exit(EXIT_SUCCESS);
}
//--------------------------------
void do_child()
{
	while(true)
	{
		int num = rand() % 10;// between 1 and 10
		if (num < 3)
		{
			printf("process %d ends\n", getpid());
			exit(EXIT_SUCCESS);// end process
		}
		else
		{
			kill(son_pid, SIGCONT);//send signal to father
			signal(SIGCONT, sig_handler);// get signal from father
			signal(SIGALRM, sig_quit);// if didnt get a signal in 5 sec
			alarm(MAX_ALARM);
			pause();
		}
	}
	exit(EXIT_SUCCESS);
}
//-------------------------------------------
void sig_handler(int sig)
{
	signal(SIGCONT, sig_handler);
	printf("process %d has a partner\n", getpid());
}
//------------------------------------
void sig_quit(int sig)
{ // process is running alone: will quit
	printf("process %d was left alone, and quits\n", getpid());
	exit (EXIT_SUCCESS);
}






