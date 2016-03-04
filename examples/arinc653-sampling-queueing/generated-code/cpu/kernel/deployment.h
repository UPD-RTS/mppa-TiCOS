#ifndef __XML_GENERATED_DEPLOYMENT_H_
#define __XML_GENERATED_DEPLOYMENT_H_

#include <core/schedvalues.h>

#define POK_NEEDS_CONSOLE	1

#define POK_NEEDS_THREADS	1

#define POK_NEEDS_PARTITIONS	1

#define POK_NEEDS_SCHED	1

#define POK_CONFIG_NB_PARTITIONS	2

#define POK_CONFIG_NB_THREADS	12

#define POK_CONFIG_PARTITIONS_NTHREADS	{5, 5}

#define POK_NEEDS_SCHED_O1	1

#define POK_CONFIG_PRIORITY_LEVELS	15

#define POK_CONFIG_PARTITIONS_SIZE	{120000, 120000}

#define POK_CONFIG_DEFINE_BASE_VADDR	1

#define POK_CONFIG_PARTITIONS_BASE_VADDR	{0x1000,0x4000}

#define POK_CONFIG_PARTITIONS_LOADADDR	{0x9f000,0xc0000}

#define POK_CONFIG_PARTITIONS_SCHEDULER	{POK_SCHED_O1, POK_SCHED_O1}

#define POK_CONFIG_SCHEDULING_SLOTS	{10,10}

#define POK_CONFIG_SCHEDULING_SLOTS_ALLOCATION	{0,1}

#define POK_CONFIG_SCHEDULING_NBSLOTS	2

#define POK_CONFIG_SCHEDULING_MAJOR_FRAME	20

#define POK_USER_STACK_SIZE	8192

#define POK_BUS_FREQ_MHZ	74

#define POK_FREQ_DIV	1

#define POK_FREQ_SHIFT	0

#define POK_DISABLE_LOADER_DATA_INIT	1

#define POK_PREFILL_PTE	1

#define POK_NEEDS_ARINC653	1

#define POK_NEEDS_PORTS_SAMPLING	1

#define POK_NEEDS_PORTS_QUEUEING	1

#define POK_NEEDS_PORTS_SLOT	1

#define POK_CONFIG_NB_PORTS	8

#define POK_CONFIG_NB_GLOBAL_PORTS	8

#define POK_CONFIG_MAX_QUEUING_MESSAGES 	2

#define POK_CONFIG_PARTITIONS_PORTS	{0,0,1,1,0,1,1,0}

#define IDLE_STACK_SIZE	4096

#define POK_CONFIG_NB_ASYNCH_EVENTS	0

#define POK_CONFIG_PARTITIONS_NB_ASYNCH_EVENTS	{0, 0}

#define POK_CONFIG_PARTITIONS_POSTWRITE_TIME	{2,3}

typedef enum
{
	SS_0 = 0,
	QD_1 = 1,
	QS_2 = 2,
	SD_3 = 3,
	QD_4 = 4,
	QD_5 = 5,
	QS_6 = 6,
	QS_7 = 7
} pok_port_local_identifier_t;

typedef enum
{
	SS_0_global = 0,
	QD_1_global = 1,
	QS_2_global = 2,
	SD_3_global = 3,
	QD_4_global = 4,
	QD_5_global = 5,
	QS_6_global = 6,
	QS_7_global = 7
} pok_port_identifier_t;

#endif