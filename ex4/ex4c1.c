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

#define      MAX_STR     100
#define      MAX_ARR     1000
#define      MSG_TYPE    1
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
	char _mtext[MAX_STR];
} ;
struct my_msgbuf {
	long mtype;
	struct Data _data ;
};
//---------------------------------------------------
void catch_int(int status);
int search_linary(int arr[], int num);
int delete_from_arr(int arr[], int max_arr, int num_to_del);
void exit_error();
//---------------------------------------------------
int end = FALSE ;
//---------------------------------------------------
int main(void)
{
	struct my_msgbuf my_msg;
	signal(SIGINT, catch_int) ;
	int msqid;
	key_t key;
	if ((key = ftok(".", 'c')) == END) {
		perror("ftok app failed");
		exit(EXIT_FAILURE);
	}
	if ((msqid = msgget(key, 0600 | IPC_CREAT| IPC_EXCL)) == END) {
		perror("msgget app d failed");
		exit(EXIT_FAILURE);
	}
	int arr_of_pids[MAX_ARR], max_arr = 0;
	while(TRUE)
	{
		if (msgrcv(msqid, &my_msg, sizeof(struct Data), MSG_TYPE, 0) == END) {
			perror("msgrcv1 failed");
		    end = TRUE;}

		if(my_msg._data._c == 'e')
		{
			max_arr = delete_from_arr(arr_of_pids, max_arr, my_msg._data._id);
			printf("server delete..\n");
		}
		printf("father got: %d\n", my_msg._data._id) ;
		if(search_linary(arr_of_pids, my_msg._data._id) == 0)
			my_msg._data._res_server = EXIST;
		else
		{
			my_msg._data._res_server = SUCCESS;
			arr_of_pids[max_arr] = my_msg._data._id;
			printf("arr[%d] = %d\n",max_arr, my_msg._data._id) ;
			max_arr++;
		}
		my_msg.mtype =  my_msg._data._id;
		if (!end && msgsnd(msqid, &my_msg, sizeof(struct Data), 0) == END)
		{
			perror("cannot send from app");
			end = TRUE;
		}
		if(end)
			if (msgctl(msqid, IPC_RMID, NULL) == END)
			{ perror("msgctl failed");
			exit(EXIT_FAILURE);
			}
		printf("max arr is: %d\n", max_arr);
	}

	my_msg.mtype = my_msg._data._id; /*1*/
	my_msg._data._res_server = FULL;
	if(msgsnd(msqid, &my_msg, sizeof(struct Data), 0) == END)
		perror("msgrcv1 failed");

	return EXIT_SUCCESS;
}
//---------------------------------------------------
void catch_int(int status)
{	end = TRUE ;}
//----------------------------------------------------------
int delete_from_arr(int arr[], int max_arr, int num_to_del)
{
	int i, j;
	for (i = 0; i < max_arr; i++)
		if (arr[i] == num_to_del)
			break;
	if (i <= max_arr)
	{
		max_arr--;
		for (j = i; j < max_arr; j++)
			arr[j] = arr[j+1];
	}
	return max_arr;
}
//---------------------------------------------------
int search_linary(int arr[], int num)
{
	for(int i = 0; i < MAX_ARR; i++)
		if (arr[i] == num)
			return 0;
	return 1;
}
