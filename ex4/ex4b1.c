/*
 * ex4b1.c
 *
 *  Created on: Dec 13, 2020
 *      Author: eliacharfe
 */

/*
 * y1.c
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
#define      MAX_MSG_LEN 1000
#define      MAX       20
#define      MAX_ARR   20
#define      MAX_RAND  20
#define      ID1       1
#define      ID2       2
#define      ID3       3
#define      TRUE      1
#define      NONE      4
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
void catch_term(int s) ;
int is_prime(int n) ;
void catch_term(int s) ;
int is_prime(int n) ;
int  delete_from_arr(int arr[], int max_arr, int num_to_del);
void quickSort(int arr[], int low, int high);
int partition (int arr[], int low, int high);
void swap(int* i, int* j);
bool bin_search(int arr[], int num);
//---------------------------------------------------
int end = 0 ;
//---------------------------------------------------
int main(void)
{
	struct my_msgbuf my_msg;
	int msqid, num_of_father_get = 0, num_of_del = 0;
	key_t key;
	signal(SIGTERM, catch_term) ;
	if ((key = ftok(".", '4')) == -1) {
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}
	if ((msqid = msgget(key, 0600 | IPC_CREAT| IPC_EXCL))
			== -1) {
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}
	int arr_of_pid[4] = {0, 0, 0};
	for (int i = 0; i < 3 ; ++i) ////////////
	{
		msgrcv(msqid,  &my_msg, MAX_MSG_LEN, 1, 0);
		arr_of_pid[i] = atoi(my_msg.mtext);
		printf ("arr[%d] : %d\n\n", i, arr_of_pid[1]);
	}
	for (int i = 1; i <= 3 ; ++i)////////////////
	{
		strcpy(my_msg.mtext, "ok");
		my_msg.mtype = i + 1;
		msgsnd(msqid, &my_msg, strlen(my_msg.mtext)+1, 0);
		printf ("server send : %s\n\n", my_msg.mtext);
	}

	int arr[MAX_ARR];
	for (int i = 0; i < MAX_ARR; i++) // build array, max = 50000
		arr[i] = rand()% MAX_RAND;// 0 -200000
	quickSort(arr, 0,  MAX_ARR-1);//sort array

	int max_arr = MAX_ARR;
	time_t time1 = time(NULL);
	while(max_arr != 0)
	{
		if (msgrcv(msqid, &my_msg,
				sizeof(struct Data), 1, 0) == -1) {
			perror("msgrcv1 failed");
			end = 1 ;
		}
		num_of_father_get++;
		my_msg.mtype = my_msg._data._id ;
		if (bin_search(arr, my_msg._data._num))
		{
			max_arr = delete_from_arr(arr,max_arr, my_msg._data._num);
			num_of_del++; // set
			my_msg._data._num = TRUE;
		}
		else
			my_msg._data._num = FALSE;
		if (!end && msgsnd(msqid, &my_msg,	sizeof(struct Data), 0) == -1) {
			perror("msgsnd1 failed");
			end = 1 ;
		}
		if (end)
			if (msgctl(msqid, IPC_RMID, NULL) == -1) {
				perror("msgctl failed");
				exit(EXIT_FAILURE);
			}
	}
	time_t time2 = time(NULL);
	time_t time = time2 - time1;
	printf("\nserver got in total: %d, num delete: %d time took: %lf\n",
			num_of_father_get, num_of_del, (double)time);
	my_msg._data._num = END;
	msgsnd(msqid, &my_msg,	sizeof(struct Data), 0);
	return EXIT_SUCCESS;
}
//---------------------------------------------------
void catch_term(int s) {
	end = 1 ;
}
//---------------------------------------------------

int is_prime(int n) {
	int div ;
	for (div = 2; div <= n/2; div++)
		if (n %div == 0)
			return 0 ;
	return 1 ;
}
//---------------------
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
	printf("\n array: \n");
	for (i = 0; i < max_arr; i++)
		printf("%d \n", arr[i]);
	return max_arr;
}
//---------------------------------------------------------------------------
bool bin_search(int arr[], int num)
{
	int first = 0;
	int last = MAX_ARR - 1;
	int middle = (first + last)/2;
	while (first <= last)
	{
		if (arr[middle] < num)
			first = middle + 1;
		else if (arr[middle] == num) {
			return true;
		}
		else
			last = middle - 1;
		middle = (first + last)/2;
	}
	return false;
}
//-------------------------------------------------
void quickSort(int arr[], int low, int high)
{
	if (low < high)
	{
		int pivot = partition(arr, low, high);
		quickSort(arr, low, pivot - 1);
		quickSort(arr, pivot + 1, high);
	}
}
//------------------------------------------------
int partition (int arr[], int low, int high)
{
	int pivot = arr[high], j;
	int i = (low - 1);
	for (j = low; j <= high - 1; j++)
	{
		if (arr[j] < pivot)
		{
			i++;
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[high]);
	return (i + 1);
}
//--------------------------------------
void swap(int* i, int* j)
{
	int temp = *i;
	*i = *j;
	*j = temp;
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
#define      MAX_MSG_LEN 1000
#define      MAX       20
#define      MAX_ARR   20
#define      MAX_RAND  20
#define      ID1       1
#define      ID2       2
#define      ID3       3
#define      TRUE      1
#define      NONE      4
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
void catch_term(int s) ;
int is_prime(int n) ;
void catch_term(int s) ;
int is_prime(int n) ;
int  delete_from_arr(int arr[], int max_arr, int num_to_del);
void quickSort(int arr[], int low, int high);
int partition (int arr[], int low, int high);
void swap(int* i, int* j);
bool bin_search(int arr[], int num);
//---------------------------------------------------
int end = 0 ;
//---------------------------------------------------
int main(void)
{
	struct my_msgbuf my_msg;
	int msqid, num_of_father_get = 0, num_of_del = 0;
	key_t key;
	signal(SIGTERM, catch_term) ;
	if ((key = ftok(".", '4')) == -1) {
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}
	if ((msqid = msgget(key, 0600 | IPC_CREAT| IPC_EXCL))
			== -1) {
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}

*/
	/*int arr_of_pid[4] = {0, 0, 0};
	for (int i = 0; i < 3 ; ++i)
	{
		msgrcv(msqid,  &my_msg, MAX_MSG_LEN, 1, 0);
		arr_of_pid[i] = atoi(my_msg.mtext);
		printf ("arr[%d] : %d\n\n", i, arr_of_pid[1]);
	}
	for (int i = 1; i <= 3 ; ++i)
	{
		strcpy(my_msg.mtext, "ok");
		my_msg.mtype = i + 1;
		msgsnd(msqid, &my_msg, strlen(my_msg.mtext)+1, 0);
		printf ("server send : %s\n\n", my_msg.mtext);
	}*/
	//sleep(1);
/*
	int arr[MAX_ARR];
	for (int i = 0; i < MAX_ARR; i++) // build array, max = 50000
		arr[i] = rand()% MAX_RAND;// 0 -200000
	quickSort(arr, 0,  MAX_ARR-1);//sort array

	int max_arr = MAX_ARR;
	time_t time1 = time(NULL);
	while(max_arr != 0)
	{
		if (msgrcv(msqid, &my_msg, sizeof(struct Data), 1, 0) == -1) {
			perror("msgrcv1 failed");
			end = 1 ;
			//exit(EXIT_FAILURE);
		}
		num_of_father_get++;
		my_msg.mtype = my_msg._data._id ;
		//my_msg._data._num = is_prime(my_msg._data._num) ;
		if (bin_search(arr, my_msg._data._num))
		{
			max_arr = delete_from_arr(arr,max_arr, my_msg._data._num);
			num_of_del++; // set
			my_msg._data._num = TRUE;
		}
		else
			my_msg._data._num = FALSE;
		if (!end && msgsnd(msqid, &my_msg,	sizeof(struct Data), 0) == -1) {
			perror("msgsnd1 failed");
			end = 1 ;
			//exit(EXIT_FAILURE);
		}
		if (end)
			if (msgctl(msqid, IPC_RMID, NULL) == -1) {
				perror("msgctl failed");
				exit(EXIT_FAILURE);
			}
	}
	time_t time2 = time(NULL);
	time_t time = time2 - time1;
	printf("\n++++ server got in total: %d, num delete: %d time took: %lf ++++\n",
			num_of_father_get, num_of_del, (double)time);
	my_msg._data._num = END;
	msgsnd(msqid, &my_msg,	sizeof(struct Data), 0);
	return EXIT_SUCCESS;
}
//---------------------------------------------------
void catch_term(int s) {
	end = 1 ;
}
//---------------------------------------------------

int is_prime(int n) {
	int div ;
	for (div = 2; div <= n/2; div++)
		if (n %div == 0)
			return 0 ;
	return 1 ;
}
//---------------------
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
	printf("\n array: \n");
	for (i = 0; i < max_arr; i++)
		printf("%d \n", arr[i]);
	return max_arr;
}
//---------------------------------------------------------------------------
bool bin_search(int arr[], int num)
{
	int first = 0;
	int last = MAX_ARR - 1;
	int middle = (first + last)/2;
	while (first <= last)
	{
		if (arr[middle] < num)
			first = middle + 1;
		else if (arr[middle] == num) {
			return true;
		}
		else
			last = middle - 1;
		middle = (first + last)/2;
	}
	return false;
}
//-------------------------------------------------
void quickSort(int arr[], int low, int high)
{
	if (low < high)
	{
		int pivot = partition(arr, low, high);
		quickSort(arr, low, pivot - 1);
		quickSort(arr, pivot + 1, high);
	}
}
//------------------------------------------------
int partition (int arr[], int low, int high)
{
	int pivot = arr[high], j;
	int i = (low - 1);
	for (j = low; j <= high - 1; j++)
	{
		if (arr[j] < pivot)
		{
			i++;
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[high]);
	return (i + 1);
}
//--------------------------------------
void swap(int* i, int* j)
{
	int temp = *i;
	*i = *j;
	*j = temp;
}



*/







/*
 * y1.c
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
#define      MAX_MSG_LEN 1000
#define      MAX       20
#define      MAX_ARR   20
#define      MAX_RAND  20
#define      ID1       1
#define      ID2       2
#define      ID3       3
#define      TRUE      1
#define      NONE      4
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
void catch_term(int s) ;
int is_prime(int n) ;
void catch_term(int s) ;
int is_prime(int n) ;
int  delete_from_arr(int arr[], int max_arr, int num_to_del);
void quickSort(int arr[], int low, int high);
int partition (int arr[], int low, int high);
void swap(int* i, int* j);
bool bin_search(int arr[], int num);
//---------------------------------------------------
int end = 0 ;
//---------------------------------------------------
int main(void)
{
	struct my_msgbuf my_msg;
	int msqid, num_of_father_get = 0, num_of_del = 0;
	key_t key;
	signal(SIGTERM, catch_term) ;
	if ((key = ftok(".", '4')) == -1) {
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}
	if ((msqid = msgget(key, 0600 | IPC_CREAT| IPC_EXCL))
			== -1) {
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}
	int arr_of_pid[4] = {0, 0, 0};
	for (int i = 0; i < 2 ; ++i) ////////////
	{
		msgrcv(msqid,  &my_msg, MAX_MSG_LEN, 1, 0);
		arr_of_pid[i] = atoi(my_msg.mtext);
		printf ("arr[%d] : %d\n\n", i, arr_of_pid[1]);
	}
	for (int i = 1; i <= 2 ; ++i)////////////////
	{
		strcpy(my_msg.mtext, "ok");
		my_msg.mtype = i + 1;
		msgsnd(msqid, &my_msg, strlen(my_msg.mtext)+1, 0);
		printf ("server send : %s\n\n", my_msg.mtext);
	}

	int arr[MAX_ARR];
	for (int i = 0; i < MAX_ARR; i++) // build array, max = 50000
		arr[i] = rand()% MAX_RAND;// 0 -200000
	quickSort(arr, 0,  MAX_ARR-1);//sort array

	int max_arr = MAX_ARR;
	time_t time1 = time(NULL);
	while(max_arr != 0)
	{
		if (msgrcv(msqid, &my_msg,
				sizeof(struct Data), 1, 0) == -1) {
			perror("msgrcv1 failed");
			end = 1 ;
		}
		num_of_father_get++;
		my_msg.mtype = my_msg._data._id ;
		if (bin_search(arr, my_msg._data._num))
		{
			max_arr = delete_from_arr(arr,max_arr, my_msg._data._num);
			num_of_del++; // set
			my_msg._data._num = TRUE;
		}
		else
			my_msg._data._num = FALSE;
		if (!end && msgsnd(msqid, &my_msg,	sizeof(struct Data), 0) == -1) {
			perror("msgsnd1 failed");
			end = 1 ;
		}
		if (end)
			if (msgctl(msqid, IPC_RMID, NULL) == -1) {
				perror("msgctl failed");
				exit(EXIT_FAILURE);
			}
	}
	time_t time2 = time(NULL);
	time_t time = time2 - time1;
	printf("\nserver got in total: %d, num delete: %d time took: %lf\n",
			num_of_father_get, num_of_del, (double)time);
	my_msg._data._num = END;
	msgsnd(msqid, &my_msg,	sizeof(struct Data), 0);
	return EXIT_SUCCESS;
}
//---------------------------------------------------
void catch_term(int s) {
	end = 1 ;
}
//---------------------------------------------------

int is_prime(int n) {
	int div ;
	for (div = 2; div <= n/2; div++)
		if (n %div == 0)
			return 0 ;
	return 1 ;
}
//---------------------
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
	printf("\n array: \n");
	for (i = 0; i < max_arr; i++)
		printf("%d \n", arr[i]);
	return max_arr;
}
//---------------------------------------------------------------------------
bool bin_search(int arr[], int num)
{
	int first = 0;
	int last = MAX_ARR - 1;
	int middle = (first + last)/2;
	while (first <= last)
	{
		if (arr[middle] < num)
			first = middle + 1;
		else if (arr[middle] == num) {
			return true;
		}
		else
			last = middle - 1;
		middle = (first + last)/2;
	}
	return false;
}
//-------------------------------------------------
void quickSort(int arr[], int low, int high)
{
	if (low < high)
	{
		int pivot = partition(arr, low, high);
		quickSort(arr, low, pivot - 1);
		quickSort(arr, pivot + 1, high);
	}
}
//------------------------------------------------
int partition (int arr[], int low, int high)
{
	int pivot = arr[high], j;
	int i = (low - 1);
	for (j = low; j <= high - 1; j++)
	{
		if (arr[j] < pivot)
		{
			i++;
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[high]);
	return (i + 1);
}
//--------------------------------------
void swap(int* i, int* j)
{
	int temp = *i;
	*i = *j;
	*j = temp;
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
#define      MAX_MSG_LEN 1000
#define      MAX       20
#define      MAX_ARR   20
#define      MAX_RAND  20
#define      ID1       1
#define      ID2       2
#define      ID3       3
#define      TRUE      1
#define      NONE      4
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
void catch_term(int s) ;
int is_prime(int n) ;
void catch_term(int s) ;
int is_prime(int n) ;
int  delete_from_arr(int arr[], int max_arr, int num_to_del);
void quickSort(int arr[], int low, int high);
int partition (int arr[], int low, int high);
void swap(int* i, int* j);
bool bin_search(int arr[], int num);
//---------------------------------------------------
int end = 0 ;
//---------------------------------------------------
int main(void)
{
	struct my_msgbuf my_msg;
	int msqid, num_of_father_get = 0, num_of_del = 0;
	key_t key;
	signal(SIGTERM, catch_term) ;
	if ((key = ftok(".", '4')) == -1) {
		perror("ftok failed");
		exit(EXIT_FAILURE);
	}
	if ((msqid = msgget(key, 0600 | IPC_CREAT| IPC_EXCL))
			== -1) {
		perror("msgget failed");
		exit(EXIT_FAILURE);
	}

*/
	/*int arr_of_pid[4] = {0, 0, 0};
	for (int i = 0; i < 3 ; ++i)
	{
		msgrcv(msqid,  &my_msg, MAX_MSG_LEN, 1, 0);
		arr_of_pid[i] = atoi(my_msg.mtext);
		printf ("arr[%d] : %d\n\n", i, arr_of_pid[1]);
	}
	for (int i = 1; i <= 3 ; ++i)
	{
		strcpy(my_msg.mtext, "ok");
		my_msg.mtype = i + 1;
		msgsnd(msqid, &my_msg, strlen(my_msg.mtext)+1, 0);
		printf ("server send : %s\n\n", my_msg.mtext);
	}*/
	//sleep(1);
/*
	int arr[MAX_ARR];
	for (int i = 0; i < MAX_ARR; i++) // build array, max = 50000
		arr[i] = rand()% MAX_RAND;// 0 -200000
	quickSort(arr, 0,  MAX_ARR-1);//sort array

	int max_arr = MAX_ARR;
	time_t time1 = time(NULL);
	while(max_arr != 0)
	{
		if (msgrcv(msqid, &my_msg, sizeof(struct Data), 1, 0) == -1) {
			perror("msgrcv1 failed");
			end = 1 ;
			//exit(EXIT_FAILURE);
		}
		num_of_father_get++;
		my_msg.mtype = my_msg._data._id ;
		//my_msg._data._num = is_prime(my_msg._data._num) ;
		if (bin_search(arr, my_msg._data._num))
		{
			max_arr = delete_from_arr(arr,max_arr, my_msg._data._num);
			num_of_del++; // set
			my_msg._data._num = TRUE;
		}
		else
			my_msg._data._num = FALSE;
		if (!end && msgsnd(msqid, &my_msg,	sizeof(struct Data), 0) == -1) {
			perror("msgsnd1 failed");
			end = 1 ;
			//exit(EXIT_FAILURE);
		}
		if (end)
			if (msgctl(msqid, IPC_RMID, NULL) == -1) {
				perror("msgctl failed");
				exit(EXIT_FAILURE);
			}
	}
	time_t time2 = time(NULL);
	time_t time = time2 - time1;
	printf("\n++++ server got in total: %d, num delete: %d time took: %lf ++++\n",
			num_of_father_get, num_of_del, (double)time);
	my_msg._data._num = END;
	msgsnd(msqid, &my_msg,	sizeof(struct Data), 0);
	return EXIT_SUCCESS;
}
//---------------------------------------------------
void catch_term(int s) {
	end = 1 ;
}
//---------------------------------------------------

int is_prime(int n) {
	int div ;
	for (div = 2; div <= n/2; div++)
		if (n %div == 0)
			return 0 ;
	return 1 ;
}
//---------------------
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
	printf("\n array: \n");
	for (i = 0; i < max_arr; i++)
		printf("%d \n", arr[i]);
	return max_arr;
}
//---------------------------------------------------------------------------
bool bin_search(int arr[], int num)
{
	int first = 0;
	int last = MAX_ARR - 1;
	int middle = (first + last)/2;
	while (first <= last)
	{
		if (arr[middle] < num)
			first = middle + 1;
		else if (arr[middle] == num) {
			return true;
		}
		else
			last = middle - 1;
		middle = (first + last)/2;
	}
	return false;
}
//-------------------------------------------------
void quickSort(int arr[], int low, int high)
{
	if (low < high)
	{
		int pivot = partition(arr, low, high);
		quickSort(arr, low, pivot - 1);
		quickSort(arr, pivot + 1, high);
	}
}
//------------------------------------------------
int partition (int arr[], int low, int high)
{
	int pivot = arr[high], j;
	int i = (low - 1);
	for (j = low; j <= high - 1; j++)
	{
		if (arr[j] < pivot)
		{
			i++;
			swap(&arr[i], &arr[j]);
		}
	}
	swap(&arr[i + 1], &arr[high]);
	return (i + 1);
}
//--------------------------------------
void swap(int* i, int* j)
{
	int temp = *i;
	*i = *j;
	*j = temp;
}



*/








