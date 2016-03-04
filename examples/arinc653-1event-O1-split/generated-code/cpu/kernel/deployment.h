#ifndef __XML_GENERATED_DEPLOYMENT_H_
#define __XML_GENERATED_DEPLOYMENT_H_

#include <core/schedvalues.h>

#define POK_NEEDS_CONSOLE	1

#define POK_NEEDS_THREADS	1

#define POK_NEEDS_PARTITIONS	1

#define POK_NEEDS_SCHED	1

#define POK_CONFIG_NB_PARTITIONS	1

#define POK_CONFIG_NB_THREADS	6

#define POK_CONFIG_PARTITIONS_NTHREADS	{4}

#define POK_NEEDS_SCHED_O1	1

#define POK_CONFIG_PRIORITY_LEVELS	30

#define POK_CONFIG_PARTITIONS_SIZE	{120000}

#define POK_CONFIG_DEFINE_BASE_VADDR	1

#define POK_CONFIG_PARTITIONS_BASE_VADDR	{0x1000}

#define POK_CONFIG_PARTITIONS_LOADADDR	{0x9f000}

#define POK_CONFIG_PARTITIONS_SCHEDULER	{POK_SCHED_O1_SPLIT}

#define POK_CONFIG_SCHEDULING_SLOTS	{40}

#define POK_CONFIG_SCHEDULING_SLOTS_ALLOCATION	{0}

#define POK_CONFIG_SCHEDULING_NBSLOTS	1

#define POK_CONFIG_SCHEDULING_MAJOR_FRAME	40

#define POK_USER_STACK_SIZE	8192

#define POK_BUS_FREQ_MHZ	74

#define POK_FREQ_DIV	1

#define POK_FREQ_SHIFT	0

#define POK_DISABLE_LOADER_DATA_INIT	1

#define POK_PREFILL_PTE	1

#define POK_NEEDS_ARINC653	1

#define IDLE_STACK_SIZE	4096

#define POK_CONFIG_NB_ASYNCH_EVENTS	0

#define POK_CONFIG_PARTITIONS_NB_ASYNCH_EVENTS	{0}

#define POK_NEEDS_LOCKOBJECTS	1

#define POK_CONFIG_NB_LOCKOBJECTS	1

#define POK_CONFIG_PARTITIONS_NLOCKOBJECTS	{1}

#define POK_NEEDS_SCHED_O1_SPLIT	1

#define POK_CONFIG_NB_SPORADIC_THREADS	1

#define POK_CONFIG_SUCCESSOR_THREADS	{FALSE, FALSE, TRUE, FALSE}

#define POK_CONFIG_SUCCESSORS_ID	{0, 2, 0, 0}

#define POK_CONFIG_PREDECESSOR_THREADS	{FALSE, TRUE, FALSE, FALSE}

#define POK_CONFIG_PREDECESSORS_EVENTS	{0, 1, 0, 0}

#define POK_CONFIG_SPORADIC_TARDINESS	{FALSE, FALSE, TRUE, FALSE, FALSE, TRUE}

#define POK_CONFIG_SPORADIC_DEADLINE	{0, 0, 420, 0}





#endif