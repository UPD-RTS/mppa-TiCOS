#include "activity.h"
#include <arinc653/types.h>
#include <arinc653/process.h>
#include <arinc653/partition.h>
#include "deployment.h"


PROCESS_ID_TYPE arinc_threads[POK_CONFIG_NB_THREADS];


int main ()
{

	PROCESS_ATTRIBUTE_TYPE tattr;
	RETURN_CODE_TYPE ret;

	printf("	part3 - Main thread\n");
	tattr.PERIOD = 400;
	tattr.BASE_PRIORITY = 8;
	tattr.ENTRY_POINT = thr3_1_job;
	CREATE_PROCESS (&(tattr), &(arinc_threads[1]), &(ret));


	START (arinc_threads[1],&(ret));

	SET_PARTITION_MODE (NORMAL, &(ret));
	return 0;
}
