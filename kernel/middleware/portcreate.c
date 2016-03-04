/*
 *  Copyright (c) 2012  University of Padua, Department of Mathematics
 *
 *  This file is free software: you may copy, redistribute and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation, either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  This file is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  This file incorporates work covered by the following copyright and
 *  permission notice:
 *
 * 		Copyright (c) 2007-2009 POK team
 *
 *		Redistribution and use in source and binary forms, with or without
 *		modification, are permitted provided that the following conditions
 *		are met:
 *
 *			* Redistributions of source code must retain the above copyright
 *			  notice, this list of conditions and the following disclaimer.
 *			* Redistributions in binary form must reproduce the above
 *			  copyright notice, this list of conditions and the following
 *			  disclaimer in the documentation and/or other materials
 *			  provided with the distribution.
 *			* Neither the name of the POK Team nor the names of its main
 *			  author (Julien Delange) or its contributors may be used to
 *			  endorse or promote products derived from this software
 *			  without specific prior written permission.
 *
 *		THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *		AS IS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *		LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *		FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *		COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *		INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *		BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *		LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *		CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *		LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *		ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *		POSSIBILITY OF SUCH DAMAGE.
 */

#if defined (POK_NEEDS_PORTS_SAMPLING) || defined (POK_NEEDS_PORTS_QUEUEING)

#include <errno.h>
#include <types.h>
#include <core/error.h>
#include <core/partition.h>
#include <middleware/port.h>
#include <middleware/queue.h>
#include <libc.h>

extern uint8_t		pok_current_partition;
extern pok_port_t	pok_ports[POK_CONFIG_NB_PORTS];
extern pok_queue_t	pok_queue;


pok_ret_t pok_port_create(	char* 				name, 
				const pok_port_size_t 		size,
				const pok_port_direction_t 	direction,
				uint8_t kind, pok_port_id_t* 	id)

{
#if defined(POK_SD) || defined(POK_QD)
	printf ("In pok_port_create");
#endif
	uint8_t	gid;
	pok_ret_t ret;

	ret = POK_ERRNO_OK;

#if defined(POK_SD) || defined(POK_QD)
	printf ("pok_port_create - 1");
#endif
	if (size > POK_PORT_MAX_SIZE)
	{
#if (defined POK_NEEDS_PARTITIONS) && (defined POK_NEEDS_ERROR_HANDLING)
		POK_ERROR_CURRENT_PARTITION(POK_ERROR_KIND_PARTITION_CONFIGURATION);
#endif
#if defined(POK_SD) || defined(POK_QD)
		printf (" (size problem) ");
#endif
		return POK_ERRNO_PORT;
	}
#if defined(POK_SD) || defined(POK_QD)
	printf ("pok_port_create - 2");
#endif
	if (size > pok_queue.available_size)
	{
#if (defined POK_NEEDS_PARTITIONS) && (defined POK_NEEDS_ERROR_HANDLING)
		POK_ERROR_CURRENT_PARTITION(POK_ERROR_KIND_PARTITION_CONFIGURATION);
#endif
#if defined(POK_SD) || defined(POK_QD)
		printf (" (available size problem) ");
#endif
		return POK_ERRNO_PORT;
	}
#if defined(POK_SD) || defined(POK_QD)
	printf ("pok_port_create - 3");
#endif
	if ((direction != POK_PORT_DIRECTION_IN) &&
		 (direction != POK_PORT_DIRECTION_OUT))
	{
#if (defined POK_NEEDS_PARTITIONS) && (defined POK_NEEDS_ERROR_HANDLING)
		POK_ERROR_CURRENT_PARTITION(POK_ERROR_KIND_PARTITION_CONFIGURATION);
#endif
#if defined(POK_SD) || defined(POK_QD)
		printf (" (direction problem) ");
#endif
		return POK_ERRNO_PORT;
	}

#if defined(POK_SD) || defined(POK_QD)
	printf ("pok_port_create - 4");
#endif
	switch (kind)
	{
		case POK_PORT_KIND_SAMPLING:
		{
#ifdef POK_NEEDS_PORTS_SAMPLING
 #ifdef POK_SD
			printf ("SAMPLING -");
 #endif
			ret = pok_port_sampling_id(name, &gid);
#endif
			break;
		}
		case POK_PORT_KIND_QUEUEING:
		{
#ifdef POK_QD
			printf ("QUEUING -");
#endif
#ifdef POK_NEEDS_PORTS_QUEUEING
			ret = pok_port_queueing_id (name, &gid);
#endif
#if defined(POK_SD) || defined(POK_QD)
			printf ("after queing\n");
#endif
				break;
		}
		default:
		{
#if defined(POK_SD) || defined(POK_QD)
			printf ("POK_ERRNO_EINVAL \n ");
#endif
			return POK_ERRNO_EINVAL;
			break;
		}
	}

#if defined(POK_SD) || defined(POK_QD)
		printf ("after 1 \n ");
#endif
	if (ret != POK_ERRNO_OK)
	{
#if (defined POK_NEEDS_PARTITIONS) && (defined POK_NEEDS_ERROR_HANDLING)
		POK_ERROR_CURRENT_PARTITION(POK_ERROR_KIND_PARTITION_INIT);
#endif
#if defined(POK_SD) || defined(POK_QD)
		printf ("after \n ");
#endif
		return ret;
	}

#if defined(POK_SD) || defined(POK_QD)
		printf ("after 2 \n ");
#endif

	if (! pok_own_port (POK_SCHED_CURRENT_PARTITION, gid))
	{
#if defined(POK_SD) || defined(POK_QD)
		printf ("Not my port \n ");
#endif
		return POK_ERRNO_PORT;
	}

#if defined(POK_SD) || defined(POK_QD)
		printf ("after 3 \n ");
#endif
	/*
	 * Check if the port was already created
	 * If it's already created, we return ERRNO_EXISTS but indicate
	 * the right port-id. This should not be taken as an assumption
	 * but could help the developer when a partition is restarted.
	 */
	if (pok_ports[gid].ready == TRUE)
	{
		*id = gid;
#if defined(POK_SD) || defined(POK_QD)
		printf ("Already exists \n ");
#endif
		return POK_ERRNO_EXISTS;
	}

#if defined(POK_SD) || defined(POK_QD)
	printf ("before assignement \n ");
#endif
	pok_ports[gid].index		= pok_queue.size - pok_queue.available_size;
	pok_ports[gid].off_b		= 0;
	pok_ports[gid].off_e		= 0;
	pok_ports[gid].size			= size;
#if defined(POK_SD) || defined(POK_QD)
	printf ("size of port %d: %d \n ",gid, size);
#endif
	pok_ports[gid].full			= FALSE;
	pok_ports[gid].partition  	= pok_current_partition;
	pok_ports[gid].direction 	= direction;
	pok_ports[gid].ready		= TRUE;
	pok_ports[gid].kind			= kind;

#ifdef POK_NEEDS_PORTS_SAMPLING
	pok_ports[gid].token.empty = TRUE;
#endif

#ifdef POK_NEEDS_PORTS_QUEUEING
	pok_ports[gid].first_size 	= -1;
	pok_ports[gid].first_empty 	= 0;
	pok_ports[gid].first_not_empty 	= -1;
	pok_ports[gid].all_tokens_empty = TRUE;
	pok_ports[gid].all_tokens_full 	= FALSE;

#endif
	pok_queue.available_size  = pok_queue.available_size - size;

	*id = gid;

#if defined(POK_SD) || defined(POK_QD)
		printf ("[size:%i] Successful (%i)\n", size, gid);
#endif
	return POK_ERRNO_OK;
}
#endif /* if defined (POK_NEEDS_PORTS_SAMPLING) || defined (POK_NEEDS_PORTS_QUEUEING) */


