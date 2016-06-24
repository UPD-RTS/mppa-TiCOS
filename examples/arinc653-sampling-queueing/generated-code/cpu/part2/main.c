#include <libc/stdio.h>
#include "activity.h"
#include <arinc653/types.h>
#include <arinc653/process.h>
#include <arinc653/partition.h>
#include "deployment.h"
#include <arinc653/sampling.h>
#include <arinc653/queueing.h>


PROCESS_ID_TYPE arinc_threads[POK_CONFIG_NB_THREADS];

// Sampling ports ids
SAMPLING_PORT_ID_TYPE SD_3_id;
// Queuing ports ids
QUEUING_PORT_ID_TYPE QS_2_id;
QUEUING_PORT_ID_TYPE QS_6_id;
QUEUING_PORT_ID_TYPE QD_5_id;

int main ()
{

	PROCESS_ATTRIBUTE_TYPE tattr;
	RETURN_CODE_TYPE ret;

	printf("	part2 - Main thread\n");
	// Sampling ports creation
	CREATE_SAMPLING_PORT ("port_04", POK_CONFIG_MAX_MESSAGE_SIZE, DESTINATION, 2000, &(SD_3_id), &(ret));
	printf("	Sampling port created! id: 4, name: port_04, core id:%d, ret:%d\n", SD_3_id, ret);

	// Queuing ports creation
	CREATE_QUEUING_PORT ("port_03", POK_CONFIG_MAX_MESSAGE_SIZE,  POK_CONFIG_PART_MAX_QUEUING_MESSAGES, SOURCE, 0, &(QS_2_id), &(ret));
	printf("	Queueing port created! id: 3, name: port_03, core id:%d, ret:%d\n", QS_2_id, ret);
	CREATE_QUEUING_PORT ("port_07", POK_CONFIG_MAX_MESSAGE_SIZE,  POK_CONFIG_PART_MAX_QUEUING_MESSAGES, SOURCE, 0, &(QS_6_id), &(ret));
	printf("	Queueing port created! id: 7, name: port_07, core id:%d, ret:%d\n", QS_6_id, ret);
	CREATE_QUEUING_PORT ("port_06", POK_CONFIG_MAX_MESSAGE_SIZE,  POK_CONFIG_PART_MAX_QUEUING_MESSAGES, DESTINATION, 0, &(QD_5_id), &(ret));
	printf("	Queueing port created! id: 6, name: port_06, core id:%d, ret:%d\n", QD_5_id, ret);

	tattr.PERIOD = 200;
	tattr.BASE_PRIORITY = 9;
	tattr.ENTRY_POINT = thr2_1_job;
	CREATE_PROCESS (&(tattr), &(arinc_threads[1]), &(ret));

	tattr.PERIOD = 200;
	tattr.BASE_PRIORITY = 8;
	tattr.ENTRY_POINT = thr2_2_job;
	CREATE_PROCESS (&(tattr), &(arinc_threads[2]), &(ret));

	tattr.PERIOD = 200;
	tattr.BASE_PRIORITY = 7;
	tattr.ENTRY_POINT = thr2_3_job;
	CREATE_PROCESS (&(tattr), &(arinc_threads[3]), &(ret));

	tattr.PERIOD = 200;
	tattr.BASE_PRIORITY = 6;
	tattr.ENTRY_POINT = thr2_4_job;
	CREATE_PROCESS (&(tattr), &(arinc_threads[4]), &(ret));


	START (arinc_threads[1],&(ret));
	START (arinc_threads[2],&(ret));
	START (arinc_threads[3],&(ret));
	START (arinc_threads[4],&(ret));

	SET_PARTITION_MODE (NORMAL, &(ret));
	return 0;
}
