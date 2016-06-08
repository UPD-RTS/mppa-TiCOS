#include <libc/stdio.h>
#include <arinc653/types.h>
#include <arinc653/time.h>
#include <arinc653/process.h>
#include <arinc653/event.h>
#include "deployment.h"


extern EVENT_ID_TYPE pok_arinc653_events_ids[1];

extern PROCESS_ID_TYPE arinc_threads[POK_CONFIG_NB_THREADS];

void* thr1_1_job ()
{
	RETURN_CODE_TYPE ret;
	printf("\t[Thread 1] Partition n.1\n");
	while (1)
	{
		printf("\t[Thread 1] Do something\n");

		RESUME (arinc_threads[2], &(ret));
		PERIODIC_WAIT (&(ret));
	}
}

void* thr1_1bis_job()
{
	int flag = 0;
	RETURN_CODE_TYPE ret;
	SYSTEM_TIME_TYPE time = APERIODIC_PROCESS;
	printf("\t[Thread 2] Partition n.1\n");
	printf("\t[Thread 2] Sporadic (wait evt)\n");

	while (1)
	{
		if (flag){
			printf ("\t[Thread 2] Thread self-suspending\n");
		} else {
			flag=1;
		}

		SUSPEND_SELF (time, &ret);
		printf ("\t[Thread 2] Resumed, going to WAIT_EVENT \n");
		WAIT_EVENT (pok_arinc653_events_ids[0], 0, &(ret));
		printf ("\t[Thread 2] Event %i is UP\n", (unsigned int) pok_arinc653_events_ids[0]);
	}
}

void* thr1_3_job ()
{
	RETURN_CODE_TYPE ret;
	printf("\t[Thread 3] Partition n.1\n");

	while (1)
	{
		printf("\t[Thread 3] Do something\n");

		printf("\t[Thread 3] Setting event UP\n");
		SET_EVENT (pok_arinc653_events_ids[0], &(ret));
		printf("\t[Thread 3] Event SET, resuming sporadic\n");
		RESUME (arinc_threads[4], &(ret));
		PERIODIC_WAIT (&(ret));
	}
}

void* thr1_3bis_job()
{
	int flag = 0;
	RETURN_CODE_TYPE ret;
	SYSTEM_TIME_TYPE time = APERIODIC_PROCESS;
	printf("\t[Thread 4] Partition n.1\n");
	printf("\t[Thread 4] Sporadic (after set evt)\n");
	while (1)
	{
		if (flag){
			printf("\t[Thread 4] Resumed, continuation after event is set\n");
			RESET_EVENT (pok_arinc653_events_ids[0], &(ret));
			printf("\t[Thread 4] Event %i is DOWN [%i]\n", (unsigned int) pok_arinc653_events_ids[0], ret);
			printf("\t[Thread 4] Thread self-suspending\n");
		} else {
			flag=1;
		}
		SUSPEND_SELF (time, &ret);
	}
}
