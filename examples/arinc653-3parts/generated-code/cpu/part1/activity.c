#include <arinc653/types.h>
#include <arinc653/time.h>
#include "deployment.h"



void* thr1_1_job ()
{
	RETURN_CODE_TYPE ret;
	while (1)
	{
		printf("Partition n. 1 - Thread n.1\n");

		/***************************************************************/
		/* Message processing code should be placed here               */
		/***************************************************************/

		PERIODIC_WAIT (&(ret));
	}
}


