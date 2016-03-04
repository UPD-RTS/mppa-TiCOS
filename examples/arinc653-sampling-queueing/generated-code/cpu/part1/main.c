#include "activity.h"
#include <arinc653/types.h>
#include <arinc653/process.h>
#include <arinc653/partition.h>
#include "deployment.h"
#include <arinc653/sampling.h>
#include <arinc653/queueing.h>


PROCESS_ID_TYPE arinc_threads[POK_CONFIG_NB_THREADS];

// Sampling ports ids
SAMPLING_PORT_ID_TYPE SS_0_id;
// Queuing ports ids
QUEUING_PORT_ID_TYPE QD_4_id;
QUEUING_PORT_ID_TYPE QS_7_id;
QUEUING_PORT_ID_TYPE QD_1_id;

int main ()
{

	PROCESS_ATTRIBUTE_TYPE tattr;
	RETURN_CODE_TYPE ret;

	printf("	part1 - Main thread\n");
	// Sampling ports creation 
	CREATE_SAMPLING_PORT ("port_01", POK_CONFIG_MAX_MESSAGE_SIZE, SOURCE, 2000, &(SS_0_id), &(ret));
	printf("	Sampling port created! id: 1, name: port_01, core id:%d, ret:%d\n", SS_0_id, ret);

	// Queuing ports creation 
	CREATE_QUEUING_PORT ("port_05", POK_CONFIG_MAX_MESSAGE_SIZE,  POK_CONFIG_PART_MAX_QUEUING_MESSAGES, DESTINATION, 0, &(QD_4_id), &(ret));
	printf("	Queueing port created! id: 5, name: port_05, core id:%d, ret:%d\n", QD_4_id, ret);
	CREATE_QUEUING_PORT ("port_08", POK_CONFIG_MAX_MESSAGE_SIZE,  POK_CONFIG_PART_MAX_QUEUING_MESSAGES, SOURCE, 0, &(QS_7_id), &(ret));
	printf("	Queueing port created! id: 8, name: port_08, core id:%d, ret:%d\n", QS_7_id, ret);
	CREATE_QUEUING_PORT ("port_02", POK_CONFIG_MAX_MESSAGE_SIZE,  POK_CONFIG_PART_MAX_QUEUING_MESSAGES, DESTINATION, 0, &(QD_1_id), &(ret));
	printf("	Queueing port created! id: 2, name: port_02, core id:%d, ret:%d\n", QD_1_id, ret);

	tattr.PERIOD = 200;
	tattr.BASE_PRIORITY = 14;
	tattr.ENTRY_POINT = thr1_1_job;
	CREATE_PROCESS (&(tattr), &(arinc_threads[1]), &(ret));

	tattr.PERIOD = 200;
	tattr.BASE_PRIORITY = 12;
	tattr.ENTRY_POINT = thr1_2_job;
	CREATE_PROCESS (&(tattr), &(arinc_threads[2]), &(ret));

	tattr.PERIOD = 200;
	tattr.BASE_PRIORITY = 10;
	tattr.ENTRY_POINT = thr1_3_job;
	CREATE_PROCESS (&(tattr), &(arinc_threads[3]), &(ret));

	tattr.PERIOD = 200;
	tattr.BASE_PRIORITY = 8;
	tattr.ENTRY_POINT = thr1_4_job;
	CREATE_PROCESS (&(tattr), &(arinc_threads[4]), &(ret));


	START (arinc_threads[1],&(ret));
	START (arinc_threads[2],&(ret));
	START (arinc_threads[3],&(ret));
	START (arinc_threads[4],&(ret));

	SET_PARTITION_MODE (NORMAL, &(ret));
	return 0;
}
