#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#define      MAX_ARR     50000
#define      MAX_RAND    200000
#define      MSG_TYPE    1
#define      MAX_CLIENT  3
#define      SUCCESS     0
#define      EXIST       1
#define      FULL        2
#define      TRUE        1
#define      FALSE       0
#define      END        -1
//---------------------------------------------------
struct Data {
	pid_t _id ;
	int _num , _res_server;
	char _c;
	char _mtext[100];
} ;
struct my_msgbuf {
	long mtype;
	struct Data _data ;
};
//---------------------------------------------------
int is_prime( int num);
int is_palindrom(char str[]);
void exit_error();
void catch_int(int status);
//---------------------------------------------------
int end = FALSE ;
//---------------------------------------------------
int main(void)
{
	struct my_msgbuf my_msg;
	//int msqid;
	signal(SIGINT, catch_int) ;
	int msqid;
	key_t key;
	if ((key = ftok(".", 'c')) == END) {
		perror("ftok client c failed");
		exit(EXIT_FAILURE);
	}
	if ((msqid = msgget(key, 0)) == END) {
		perror("msgget client c failed");
		exit(EXIT_FAILURE);
	}
	int msqid2;
	key_t key2;
	if ((key2 = ftok(".", 'd')) == END) {
		perror("ftok app failed");
		exit(EXIT_FAILURE);
	}
	if ((msqid2 = msgget(key2, 0600 | IPC_CREAT| IPC_EXCL)) == END) {
		perror("msgget app d failed");
		exit(EXIT_FAILURE);
	}

	while(TRUE)
	{
		if (msgrcv(msqid2, &my_msg, sizeof(struct Data), 1, 0) == END)
		{
			perror("msgrcv1 failed");
			end = 1;}
		    my_msg.mtype =  my_msg._data._id;

		if (!end && msgsnd(msqid, &my_msg, sizeof(struct Data), 0) == END)
		{
			perror("cannot send from app\n");
			end = 1;
		}
		if (my_msg._data._res_server != SUCCESS)
		{
			end = TRUE; // will teminate
			my_msg._data._num = END;
			printf("app remove queue \n");
		}

		if (my_msg._data._c == 'n')
			my_msg._data._num = is_prime(my_msg._data._num) ;
		else
			my_msg._data._num = is_palindrom(my_msg._data._mtext); // c = 's'

		if (!end && msgsnd(msqid2, &my_msg, sizeof(struct Data), 0) == END)
		{
			perror("cannot send from app");
			end = TRUE;
		}

		if(end)
			if (msgctl(msqid2, IPC_RMID, NULL) == END)
			{ perror("msgctl failed");
			exit(EXIT_FAILURE);
			}
	}
	return EXIT_SUCCESS;
}
//---------------------------------------------------
int is_prime( int num)
{
	int div ;
	for (div = 2; div <= num/2; div++)
		if (num % div == 0) {
			return 0;
		}
	return 1;
	printf("%d is prime\n", num) ;
}
//---------------------------------------------------
int is_palindrom(char str[])
{
	int length = strlen(str) - 1;
	int index = 0;
	while (length > index)
	{
		if (str[index++] != str[length--])
			return 0;
	}
	return 1;
}
//---------------------------------------------------
void catch_int(int status)
{	end = TRUE ;}
