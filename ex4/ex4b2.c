/*
 * y2.c
 *
 *  Created on: Dec 13, 2020
 *      Author: eliacharfe
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h> // sleep()
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
//-----------------
#define      MAX_MSG_LEN 1000
#define      MAX       20
#define      MAX_ARR   20
#define      MAX_RAND  20
#define      ID1       1
#define      ID2       2
#define      ID3       3
#define      NONE      4
#define      TRUE      1
#define      FALSE     0
#define      END       -1
//---------------------------------------------------
struct Data {
	pid_t _id ;
	int _num ;
} ;
struct my_msgbuf {
	long mtype;
	char mtext[MAX_MSG_LEN];
	struct Data _data ;
};
//---------------------------------------------------
int main(int argc, char *argv[])
{
	struct my_msgbuf my_msg;
	int msqid;
	key_t key;
	srand(time(NULL)) ;
	if ((key = ftok(".", '4')) == -1) {
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}
	if ((msqid = msgget(key, 0)) == -1) {
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}

	my_msg.mtype = 1;
	sprintf(my_msg.mtext, "%d", getpid());
	msgsnd(msqid, &my_msg, strlen(my_msg.mtext)+1, 0);
	printf ("msgsnd to sever : %s \n\n", my_msg.mtext);

	msgrcv(msqid, &my_msg, MAX_MSG_LEN, 0, 0) ;
	printf ("****start id: %d, pid: %d \n", atoi(argv[1]), (int)getpid());

	srand(atoi(argv[1]));
	int child_send = 0, father_delete = 0;
	while (TRUE)
	{
		my_msg.mtype = 1 ;
		my_msg._data._id = getpid() ;
		my_msg._data._num = rand() %  MAX_RAND ;

		if (msgsnd(msqid, &my_msg,
				sizeof(struct Data), 0) == -1) {
			perror("msgsnd2 failed");
			exit(EXIT_FAILURE);
		}
		printf("id: %d pid: %d send %d to server\n", (int)my_msg.mtype,(int) getpid(), my_msg._data._num);
		child_send++;

		if (msgrcv(msqid, &my_msg,
				sizeof(struct Data), (int) getpid(), 0) == -1) {
			perror("msgrcv2 failed");
			exit(EXIT_FAILURE);
		}
		if (my_msg._data._num == END)
			break;

		if (my_msg._data._num == 1)
			father_delete++;
		printf("%d get %d from server\n", (int) getpid(), 	my_msg._data._num) ;
	}
	printf("\nchild: %d send: %d fath del: %d\n", (int)getpid(), child_send, father_delete);
	return EXIT_SUCCESS;
}































/*



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <unistd.h> // sleep()
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
//-----------------
#define      MAX_MSG_LEN 1000
#define      MAX       20
#define      MAX_ARR   20
#define      MAX_RAND  20
#define      ID1       1
#define      ID2       2
#define      ID3       3
#define      NONE      4
#define      TRUE      1
#define      FALSE     0
#define      END       -1
//---------------------------------------------------
struct Data {
	pid_t _id ;
	int _num ;
} ;
struct my_msgbuf {
	long mtype;
	char mtext[MAX_MSG_LEN];
	struct Data _data ;
};
//---------------------------------------------------
int main(int argc, char *argv[])
{
	struct my_msgbuf my_msg;
	int msqid;
	key_t key;
	srand(time(NULL)) ;
	if ((key = ftok(".", '4')) == -1) {
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}
	if ((msqid = msgget(key, 0)) == -1) {
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}
*/
	/*my_msg.mtype = 1;
	//my_msg._data._id = getpid();
	sprintf(my_msg.mtext, "%d", getpid());
	msgsnd(msqid, &my_msg, strlen(my_msg.mtext)+1, 0);
	printf ("msgsnd to sever : %s \n\n", my_msg.mtext);

	msgrcv(msqid, &my_msg, MAX_MSG_LEN, 0, 0) ;
	printf ("****start id: %d, pid: %d \n", atoi(argv[1]), (int)getpid());*/

/*
	int child_send = 0, father_delete = 0;
	srand(atoi(argv[1]));
	while (TRUE)
	{
		my_msg.mtype = atoi(argv[1]) ;
		my_msg._data._id = getpid() ;
		my_msg._data._num = rand() %  MAX_RAND ;
		printf("id: %d pid: %d send %d to server\n", (int)my_msg.mtype,(int) getpid(), my_msg._data._num);
		printf("%d send %d to server\n", (int) getpid(),
				my_msg._data._num) ;

		if (msgsnd(msqid, &my_msg, 	sizeof(struct Data), 0) == -1) {
			perror("msgsnd2 failed");
			exit(EXIT_FAILURE);
		}
		child_send++;

		if (msgrcv(msqid, &my_msg, 	sizeof(struct Data), (int) getpid(), 0) == -1) {
			perror("msgrcv2 failed");
			exit(EXIT_FAILURE);
		}
		if (my_msg._data._num == END)
			break;

		if (my_msg._data._num == 1)
			father_delete++;
		printf("%d get %d from server\n", (int) getpid(), 	my_msg._data._num) ;
	}
	printf("\n////child: send: %d fath del: %d///\n", child_send, father_delete);

	return EXIT_SUCCESS;
}



 */


