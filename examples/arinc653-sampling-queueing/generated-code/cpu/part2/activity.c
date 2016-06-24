#include <libc/stdio.h>
#include <arinc653/types.h>
#include <arinc653/time.h>
#include <arinc653/sampling.h>
#include <arinc653/queueing.h>
#include "deployment.h"


// Sampling ports ids, msg and msg size
extern SAMPLING_PORT_ID_TYPE SD_3_id;
MESSAGE_ADDR_TYPE SD_3_msg;
MESSAGE_SIZE_TYPE SD_3_msg_size;

// Queuing ports ids, msg and msg size
extern QUEUING_PORT_ID_TYPE QS_2_id;
MESSAGE_ADDR_TYPE QS_2_msg;
MESSAGE_SIZE_TYPE QS_2_msg_size;

extern QUEUING_PORT_ID_TYPE QS_6_id;
MESSAGE_ADDR_TYPE QS_6_msg;
MESSAGE_SIZE_TYPE QS_6_msg_size;

extern QUEUING_PORT_ID_TYPE QD_5_id;
MESSAGE_ADDR_TYPE QD_5_msg;
MESSAGE_SIZE_TYPE QD_5_msg_size;


void* thr2_1_job ()
{
	RETURN_CODE_TYPE ret;
	while (1)
	{
		printf("Partition n. 2 - Thread n.1\n");

		/***************************************************************/
		/* Message processing code should be placed here               */
		/***************************************************************/

		RECEIVE_QUEUING_MESSAGE (QD_5_id, 0, QD_5_msg, &QD_5_msg_size, &(ret));
		if (ret == 0){
			printf("  RECEIVED message   0x%x  from port 6, size %d, ret: %i\n",((unsigned char *)QD_5_msg)[0], QD_5_msg_size, ret);
		}else{
			printf("	Port empty or RECEIVE error: %d\n\n", ret);
		}

		PERIODIC_WAIT (&(ret));
	}
}

void* thr2_2_job ()
{
	RETURN_CODE_TYPE ret;
	unsigned char val7 = 7;
	QS_6_msg = &val7;
	QS_6_msg_size = sizeof(unsigned char);


	while (1)
	{
		printf("Partition n. 2 - Thread n.2\n");

		/***************************************************************/
		/* Message processing code should be placed here               */
		/***************************************************************/

		SEND_QUEUING_MESSAGE (QS_6_id, QS_6_msg, QS_6_msg_size, 0, &(ret));
		printf("  SENT message 0x%x to port 7, size: %d, ret: %d\n", ((unsigned char *)QS_6_msg)[0], QS_6_msg_size, ret);

		PERIODIC_WAIT (&(ret));
	}
}

void* thr2_3_job ()
{
	RETURN_CODE_TYPE ret;
	VALIDITY_TYPE valid;
	while (1)
	{
		printf("Partition n. 2 - Thread n.3\n");

		/***************************************************************/
		/* Message processing code should be placed here               */
		/***************************************************************/

		READ_SAMPLING_MESSAGE (SD_3_id, SD_3_msg, &SD_3_msg_size, &(valid), &(ret));
		if (ret == 0){
			printf(" READ message: 0x%x from port 4, message size: %d, ret:%i\n",((unsigned char *)SD_3_msg)[0], SD_3_msg_size, ret);
		}else{
			printf("	Port empty or READ error:%d\n\n", ret);
		}

		PERIODIC_WAIT (&(ret));
	}
}

void* thr2_4_job ()
{
	RETURN_CODE_TYPE ret;
	unsigned char val3 = 3;
	QS_2_msg = &val3;
	QS_2_msg_size = sizeof(unsigned char);


	while (1)
	{
		printf("Partition n. 2 - Thread n.4\n");

		/***************************************************************/
		/* Message processing code should be placed here               */
		/***************************************************************/

		SEND_QUEUING_MESSAGE (QS_2_id, QS_2_msg, QS_2_msg_size, 0, &(ret));
		printf("  SENT message 0x%x to port 3, size: %d, ret: %d\n", ((unsigned char *)QS_2_msg)[0], QS_2_msg_size, ret);

		PERIODIC_WAIT (&(ret));
	}
}
