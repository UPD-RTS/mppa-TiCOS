#include <arinc653/types.h>
#include <arinc653/time.h>
#include <arinc653/sampling.h>
#include <arinc653/queueing.h>
#include "deployment.h"


// Sampling ports ids, msg and msg size
extern SAMPLING_PORT_ID_TYPE SS_0_id;
MESSAGE_ADDR_TYPE SS_0_msg;
MESSAGE_SIZE_TYPE SS_0_msg_size;

// Queuing ports ids, msg and msg size
extern QUEUING_PORT_ID_TYPE QD_4_id;
MESSAGE_ADDR_TYPE QD_4_msg;
MESSAGE_SIZE_TYPE QD_4_msg_size;

extern QUEUING_PORT_ID_TYPE QS_7_id;
MESSAGE_ADDR_TYPE QS_7_msg;
MESSAGE_SIZE_TYPE QS_7_msg_size;

extern QUEUING_PORT_ID_TYPE QD_1_id;
MESSAGE_ADDR_TYPE QD_1_msg;
MESSAGE_SIZE_TYPE QD_1_msg_size;


void* thr1_1_job ()
{
	RETURN_CODE_TYPE ret;
	unsigned char val1 = 1;
	SS_0_msg = &val1;
	SS_0_msg_size = sizeof(unsigned char);


	while (1)
	{
		printf("Partition n. 1 - Thread n.1\n");

		/***************************************************************/
		/* Message processing code should be placed here               */
		/***************************************************************/

		WRITE_SAMPLING_MESSAGE (SS_0_id, SS_0_msg, SS_0_msg_size, &(ret));
		printf("  WRITTEN message 0x%x  to port 1, size: %d, ret: %d\n", ((unsigned char *)SS_0_msg)[0], SS_0_msg_size, ret);

		PERIODIC_WAIT (&(ret));
	}
}

void* thr1_2_job ()
{
	RETURN_CODE_TYPE ret;
	while (1)
	{
		printf("Partition n. 1 - Thread n.2\n");

		/***************************************************************/
		/* Message processing code should be placed here               */
		/***************************************************************/

		RECEIVE_QUEUING_MESSAGE (QD_1_id, 0, QD_1_msg, &QD_1_msg_size, &(ret));
		if (ret == 0){
			printf("  RECEIVED message   0x%x  from port 2, size %d, ret: %i\n",((unsigned char *)QD_1_msg)[0], QD_1_msg_size, ret);
		}else{
			printf("	Port empty or RECEIVE error: %d\n\n", ret);
		}

		PERIODIC_WAIT (&(ret));
	}
}

void* thr1_3_job ()
{
	RETURN_CODE_TYPE ret;
	unsigned char val8 = 8;
	QS_7_msg = &val8;
	QS_7_msg_size = sizeof(unsigned char);


	while (1)
	{
		printf("Partition n. 1 - Thread n.3\n");

		/***************************************************************/
		/* Message processing code should be placed here               */
		/***************************************************************/

		SEND_QUEUING_MESSAGE (QS_7_id, QS_7_msg, QS_7_msg_size, 0, &(ret));
		printf("  SENT message 0x%x to port 8, size: %d, ret: %d\n", ((unsigned char *)QS_7_msg)[0], QS_7_msg_size, ret);

		PERIODIC_WAIT (&(ret));
	}
}

void* thr1_4_job ()
{
	RETURN_CODE_TYPE ret;
	while (1)
	{
		printf("Partition n. 1 - Thread n.4\n");

		/***************************************************************/
		/* Message processing code should be placed here               */
		/***************************************************************/

		RECEIVE_QUEUING_MESSAGE (QD_4_id, 0, QD_4_msg, &QD_4_msg_size, &(ret));
		if (ret == 0){
			printf("  RECEIVED message   0x%x  from port 5, size %d, ret: %i\n",((unsigned char *)QD_4_msg)[0], QD_4_msg_size, ret);
		}else{
			printf("	Port empty or RECEIVE error: %d\n\n", ret);
		}

		PERIODIC_WAIT (&(ret));
	}
}


