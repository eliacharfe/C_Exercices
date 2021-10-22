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

#define      MSG_TYPE1   1
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
void exit_error();
//---------------------------------------------------
int end = FALSE ;
//---------------------------------------------------
int main(void)
{
	struct my_msgbuf my_msg;
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
		perror("ftok cliend d failed");
		exit(EXIT_FAILURE);
	}
	if ((msqid2 = msgget(key2, 0)) == END) {
		perror("msgget client d failed");
		exit(EXIT_FAILURE);
	}

	my_msg.mtype = MSG_TYPE1;
	my_msg._data._id = getpid();
	my_msg._data._num = rand()%100;
	if(msgsnd(msqid, &my_msg, sizeof(struct Data), 0) == END)
		perror("msgsnd client failed\n");
	if (msgrcv(msqid, &my_msg, sizeof(struct Data),(int)getpid(), 0) == END)
	{perror("msgrcv client failed");}

	if (my_msg._data._res_server == SUCCESS)
		printf("now exist\n");
	else
		if (my_msg._data._res_server == FULL
		   || (my_msg._data._res_server == EXIST))
		{
			perror("client terminate, no room or exist already\n");
			exit (EXIT_SUCCESS);
		}

	for (int i = 0; i < 100; i++) // while (scanf("%c", &c) != 'e')
	{
		my_msg.mtype = MSG_TYPE1;
		my_msg._data._id = getpid();
		char c;
		printf("Enter n or s or e: \n");
		scanf("%c", &c);
		switch(c)
		{
		case 'n':
			printf("Enter number: \n");
			scanf("%d", &my_msg._data._num);
			printf("The number: %d is ", my_msg._data._num );
			my_msg._data._c = 'n';
			break;
		case 's':
			printf("Enter string to check palindrom: \n");
			scanf("%s", my_msg._data._mtext);
			printf("string %s is:  ", my_msg._data._mtext);
			my_msg._data._c = 's';
			break;
		case 'e':
			my_msg._data._c = 'e';
			if(msgsnd(msqid, &my_msg, sizeof(struct Data), 0) == END)
					perror("msgsnd client failed\n"); // to server
			exit (EXIT_FAILURE);
			break;
		}
		if(msgsnd(msqid2, &my_msg, sizeof(struct Data), 0) == END)
		{ perror("msgsnd client failed\n"); // to app
		exit(EXIT_FAILURE);	}
		if (msgrcv(msqid2, &my_msg, sizeof(struct Data),(int)getpid(), 0) == END)
		{
			perror("msgrcv client failed"); // from app
		    exit (EXIT_FAILURE); }

		if (c == 'n')
			(my_msg._data._num == FALSE) ? printf("not prime\n") :
					printf("prime\n");
		else if(c =='s')
			(my_msg._data._num == FALSE) ? printf("not plindrom\n") :
					printf("palindrom\n");
	}

	return EXIT_SUCCESS;
}
