#include <libc/stdio.h>
#include <arinc653/types.h>
#include <arinc653/time.h>
#include <arinc653/process.h>
#include <arinc653/event.h>
#include "deployment.h"


extern EVENT_ID_TYPE pok_arinc653_events_ids[1];

extern PROCESS_ID_TYPE arinc_threads[POK_CONFIG_NB_THREADS];


void* thr1_1_pre_wait1_job ()
{
	RETURN_CODE_TYPE ret;
	while (1)
	{
		printf("Partition n. 1 - Thread n.1\n");

		/***************************************************************/
		/* Message processing code should be placed here               */
		/***************************************************************/

		printf("Partition n. 1 - Thread n. 1 : predecessor of wait1-thread\n");
		printf("\t-- do pre-wait1 activity\n");
		printf("\t-- end predecessor (wait1) activity, suspend itself waiting for the next period!\n");
		PERIODIC_WAIT (&(ret));
	}
}

void* thr1_2_succ_wait1_job()
{
	RETURN_CODE_TYPE ret;
	while (1)
	{
		printf(" Partition n. 1 - Thread n. 2 : successor of wait1-thread\n");
		printf("\t-- suspend itself waiting for activation event\n");
		WAIT_EVENT (pok_arinc653_events_ids[0], 0, &(ret));
		printf(" Partition n. 1 - Thread n. 2 : successor of wait1-thread\n");
		printf("\t-- after activation event (event1)! Do post-wait activity\n");
		RESET_EVENT (pok_arinc653_events_ids[0], &(ret));
	}
}

void* thr1_3_set1_job ()
{
	RETURN_CODE_TYPE ret;
	while (1)
	{
		printf("Partition n. 1 - Thread n.3\n");

		/***************************************************************/
		/* Message processing code should be placed here               */
		/***************************************************************/

		printf(" Partition n. 1 - Thread n. 3 : set1-thread\n");
		printf("\t-- setting UP the event1\n");
		SET_EVENT (pok_arinc653_events_ids[0], &(ret));
		printf("\t-- end setter (event1) activity, suspend itself waiting for the next period!\n");
		PERIODIC_WAIT (&(ret));
	}
}
