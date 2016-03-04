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

/**
 * \file	 middleware/portutils.c
 * \brief	Various functions for ports management.
 */

#if defined (POK_NEEDS_PORTS_SAMPLING) || defined (POK_NEEDS_PORTS_QUEUEING)

#include <types.h>
#include <libc.h>
#include <core/time.h>
#include <middleware/port.h>
#include <middleware/queue.h>

extern pok_port_t	pok_ports[POK_CONFIG_NB_PORTS];
extern pok_queue_t	pok_queue;
extern uint8_t		pok_current_partition;

const uint32_t		partition_ports [POK_CONFIG_NB_PORTS] = POK_CONFIG_PARTITIONS_PORTS;

pok_port_size_t pok_port_available_size (uint8_t pid)
{
	if (pok_ports[pid].full == TRUE)
	{
		return 0;
	}

 #if defined(POK_SD) || defined(POK_QD)
		printf (" port %d, size %d, offb %d, offe %d \n ",pid, pok_ports[pid].size,pok_ports[pid].off_b, 
			pok_ports[pid].off_e);
 #endif
	if (pok_ports[pid].off_b > pok_ports[pid].off_e)
	{
 		return (pok_ports[pid].off_b - pok_ports[pid].off_e);
	}
	else
	{
		return (pok_ports[pid].size - (pok_ports[pid].off_e - pok_ports[pid].off_b));
	}
}

pok_port_size_t pok_port_consumed_size (uint8_t pid)
{
	if (pok_ports[pid].empty == TRUE)
	{
		return 0;
	}

	if (pok_ports[pid].off_b < pok_ports[pid].off_e )
	{
		return (pok_ports[pid].off_e - pok_ports[pid].off_b);
	}
	else
	{
		return (pok_ports[pid].size - pok_ports[pid].off_b + pok_ports[pid].off_e);
	}
}


// return data of the specifed size in the queueing/sampling
pok_ret_t pok_port_get (const uint8_t pid, void *data, const pok_port_size_t size)
{
 #ifdef POK_NEEDS_PORTS_QUEUEING
	pok_port_size_t tmp_size;
	pok_port_size_t tmp_size2;
 #endif

	switch (pok_ports[pid].kind)
	{

 #ifdef POK_NEEDS_PORTS_QUEUEING
		case POK_PORT_KIND_QUEUEING:
			if (pok_ports[pid].empty == TRUE)
			{
				return POK_ERRNO_EINVAL;
			}

			if (pok_ports[pid].size < size)
			{
				return POK_ERRNO_SIZE;
			}

			if ((pok_ports[pid].off_b + size) > pok_ports[pid].size)
			{
				tmp_size = pok_ports[pid].size - pok_ports[pid].off_b;
				memcpy (data, &pok_queue.data[pok_ports[pid].index + pok_ports[pid].off_b], tmp_size);
				tmp_size2 = size - tmp_size;
				memcpy (data + tmp_size, &pok_queue.data[pok_ports[pid].index], tmp_size2);
			}
			else
			{
				memcpy (data, &pok_queue.data[pok_ports[pid].index + pok_ports[pid].off_b], size);
			}

			pok_ports[pid].off_b = (pok_ports[pid].off_b + size) % pok_ports[pid].size;

			if (pok_ports[pid].off_b == pok_ports[pid].off_e)
			{
				pok_ports[pid].empty = TRUE;
				pok_ports[pid].full  = FALSE;
			}

			return POK_ERRNO_OK;
			break;
 #endif // POK_NEEDS_PORTS_QUEUEING

 #ifdef POK_NEEDS_PORTS_SAMPLING
		case POK_PORT_KIND_SAMPLING:
			if (pok_ports[pid].empty == TRUE)
			{
				return POK_ERRNO_EMPTY;
			}

			if (size > pok_ports[pid].size)
			{
				return POK_ERRNO_SIZE;
			}

			memcpy (data, &pok_queue.data[pok_ports[pid].index + pok_ports[pid].off_b], size);

			return POK_ERRNO_OK;
			break;
 #endif // POK_NEEDS_PORTS_SAMPLING

		default:
			return POK_ERRNO_EINVAL;
	}
}


/*
* read one message at a time,
* size returns the length of the read message
*/
pok_ret_t pok_port_get_message (const uint8_t pid, void *data,  pok_port_size_t * size)
{
 #if defined(POK_SD) || defined(POK_QD)
	printf ("\nTrying to read from port %i\n", pid);
 #endif
 #ifdef POK_NEEDS_PORTS_QUEUEING
	pok_port_size_t tmp_size;
	pok_port_size_t tmp_size2;
 #endif
	switch (pok_ports[pid].kind)
	{
 #ifdef POK_NEEDS_PORTS_QUEUEING
		case POK_PORT_KIND_QUEUEING:
			if (pok_ports[pid].empty == TRUE)
			{
				return POK_ERRNO_EINVAL;
			}
  			int16_t first = pok_ports[pid].first_size;
			*size = pok_ports[pid].msg_sizes[first];
  #if defined(POK_SD) || defined(POK_QD)
			printf("----first message to read %d\n",first);		
			printf("----size of firt message %d\n",*size);
  #endif		
			// % operation should be necessary only in case the flushall procedure at the end of the partition is never executed. 
			// pok_ports[pid].msg_sizes.first = (pok_ports[pid].first_size + 1) % POK_CONFIG_MAX_QUEUING_MESSAGES;
			pok_ports[pid].first_size++;
			pok_ports[pid].nb_messages--;
 
			if ((pok_ports[pid].off_b + *size) > pok_ports[pid].size)
			{
				tmp_size = pok_ports[pid].size - pok_ports[pid].off_b;
				memcpy (data, &pok_queue.data[pok_ports[pid].index + pok_ports[pid].off_b], tmp_size);
				tmp_size2 = *size - tmp_size;
				memcpy (data + tmp_size, &pok_queue.data[pok_ports[pid].index], tmp_size2);
			}
			else
			{
				memcpy (data, &pok_queue.data[pok_ports[pid].index + pok_ports[pid].off_b], *size);
			}
  #if defined(POK_SD) || defined(POK_QD)
			printf("----message to read  %c\n",pok_queue.data[pok_ports[pid].index + pok_ports[pid].off_b]);	
			printf("----message read %c\n",((unsigned char *)data)[0]);
  #endif
			pok_ports[pid].off_b = (pok_ports[pid].off_b + *size) % pok_ports[pid].size;

			if (pok_ports[pid].off_b == pok_ports[pid].off_e)
			{
				pok_ports[pid].empty = TRUE;
				pok_ports[pid].full  = FALSE;
				pok_ports[pid].first_size=-1;
			}

			return POK_ERRNO_OK;
		break;
 #endif // POK_NEEDS_PORTS_QUEUEING



 #ifdef POK_NEEDS_PORTS_SAMPLING
		case POK_PORT_KIND_SAMPLING:
			if (pok_ports[pid].empty == TRUE)
			{

  #if defined(POK_SD) || defined(POK_QD)						
	printf ("\n** PORT IS EMPTY **\n");
  #endif
				return POK_ERRNO_EMPTY;
			}

			memcpy (data, &pok_queue.data[pok_ports[pid].index + pok_ports[pid].off_b], pok_ports[pid].off_e);	 
		// return the right size
			*size = pok_ports[pid].off_e;

			return POK_ERRNO_OK;
			break;

 #endif // sampling

		default:
			return POK_ERRNO_EINVAL;
	}
}


pok_ret_t  pok_port_write (const uint8_t pid, const void *data, const pok_port_size_t size)
{
 #ifdef POK_NEEDS_PORTS_QUEUEING
	pok_port_size_t tmp_size;
	pok_port_size_t tmp_size2;
 #endif

 #if defined(POK_SD) || defined(POK_QD)						
	printf ("\nTrying to write to port %i\n", pid);
 #endif

	switch (pok_ports[pid].kind)
	{
 #ifdef POK_NEEDS_PORTS_QUEUEING
		case POK_PORT_KIND_QUEUEING:
			if (pok_ports[pid].full == TRUE)
			{
				return POK_ERRNO_SIZE;
			}

			if (size > pok_ports[pid].size)
			{
				return POK_ERRNO_SIZE;
			}

 #if defined(POK_SD) || defined(POK_QD)
			printf ("port id: %d, port.index %d, port.off_b %d, port.off_e %d, ", 
			pid, pok_ports[pid].index, pok_ports[pid].off_b, pok_ports[pid].off_e);
 #endif
			if ((pok_ports[pid].off_e + size) > pok_ports[pid].size)
			{
				tmp_size = pok_ports[pid].size - pok_ports[pid].off_e;
				memcpy (&pok_queue.data[pok_ports[pid].index + pok_ports[pid].off_e], data, tmp_size);

				tmp_size2 = size - tmp_size;
				memcpy (&pok_queue.data[pok_ports[pid].index], data + tmp_size, tmp_size2);
			}
			else
			{
				memcpy (&pok_queue.data[pok_ports[pid].index + pok_ports[pid].off_e], data, size);
			}

			pok_ports[pid].off_e = (pok_ports[pid].off_e + size) % pok_ports[pid].size;

			if (pok_ports[pid].off_e == pok_ports[pid].off_b)
			{
				pok_ports[pid].full = TRUE;
			}

			pok_ports[pid].empty = FALSE;

		// % operation should be necessary only in case the flushall procedure at the end of the partition is never executed. 
		//pok_ports[pid].nb_messages = (pok_ports[pid].nb_messages +1) % POK_CONFIG_MAX_QUEUING_MESSAGES;
			if (pok_ports[pid].first_size <0){
				pok_ports[pid].first_size = 0;
			}		
			pok_ports[pid].msg_sizes[pok_ports[pid].nb_messages] = size;
			pok_ports[pid].nb_messages++;
			return POK_ERRNO_OK;

		break;
#endif

#ifdef POK_NEEDS_PORTS_SAMPLING
		case POK_PORT_KIND_SAMPLING:
			if (size > pok_ports[pid].size)
			{
 #ifdef POK_SD
				printf ("pok_port_write: incompatible size -- size: %d, pok_ports[pid].size: %d\n",size, pok_ports[pid].size);
 #endif		
				return POK_ERRNO_SIZE;
			}

			memcpy (&pok_queue.data[pok_ports[pid].index], data, size);
			pok_ports[pid].off_e = size;

	 	 	pok_ports[pid].empty = FALSE;

 #if defined(POK_SD) || defined(POK_QD)
			printf ("Port %i is not empty\n", pid);
 #endif				
			pok_ports[pid].last_receive = POK_GETTICK ();
			return POK_ERRNO_OK;
		 break;
#endif
		default:
			return POK_ERRNO_EINVAL;
	}
}


/*
 * This function is designed to transfer data from one port to another
 * It is called when we transfer all data from one partition to the
 * others.
 */
pok_ret_t pok_port_transfer (const uint8_t pid_dst, const uint8_t pid_src)
{
	pok_port_size_t len = 0;
	pok_port_size_t src_len_consumed = 0;

	if (pok_ports[pid_src].empty == TRUE)
	{
		return POK_ERRNO_EMPTY;
	}

	if (pok_ports[pid_src].kind == POK_PORT_KIND_QUEUEING)
	{
		len = pok_port_available_size (pid_dst);
	}
	else
	{
		if (pok_ports[pid_src].size != pok_ports[pid_dst].size)
		{
			return POK_ERRNO_SIZE;
		}

		len = pok_ports[pid_src].size;
	}

	if (pok_ports[pid_src].kind == POK_PORT_KIND_QUEUEING)
	{
		src_len_consumed = pok_port_consumed_size (pid_src);

		if (src_len_consumed == 0)
		{
			return POK_ERRNO_SIZE;
		}

		/*
		 * Here, we check the size of data produced in the source port.
		 * If there is more free space in the destination port, the size
		 * of copied data will be the occupied size in the source port.
		 */
		if (len > src_len_consumed)
		{
			len = src_len_consumed;
		}
	}

	if (len == 0)
	{
		return POK_ERRNO_SIZE;
	}
	/*
	 * Len is the size to copy. If size is null, it's better to return
	 * directly
	 */

	memcpy (&pok_queue.data[pok_ports[pid_dst].index + pok_ports[pid_dst].off_e], &pok_queue.data[pok_ports[pid_src].index + pok_ports[pid_src].off_b], len);

	if (pok_ports[pid_src].kind == POK_PORT_KIND_QUEUEING)
	{
		pok_ports[pid_dst].off_e =  (pok_ports[pid_dst].off_e + len) % pok_ports[pid_dst].size;
		pok_ports[pid_src].off_b =  (pok_ports[pid_src].off_b + len) % pok_ports[pid_src].size;

		if (pok_ports[pid_src].off_b == pok_ports[pid_src].off_e)
		{
			pok_ports[pid_src].empty = TRUE;
			pok_ports[pid_src].full  = FALSE;
		}
	}
	else
	{
		pok_ports[pid_src].empty = TRUE;
	}

	pok_ports[pid_src].full  = FALSE;
	pok_ports[pid_dst].empty = FALSE;

	return POK_ERRNO_OK;
}

bool_t pok_own_port (const uint8_t partition, const uint8_t port)
{
#if defined(POK_SD) || defined(POK_QD)
		printf ("\nOWN_PORT: port %i, part %i\n",port, partition);
 #ifdef POK_CONFIG_PARTITIONS_PORTS
		printf ("POK_CONFIG_PARTITIONS_PORTS is defined\n");
 #else
		printf ("POK_CONFIG_PARTITIONS_PORTS is NOT defined\n");
 #endif

#endif

	if (port > POK_CONFIG_NB_PORTS)
	{
		printf ("OWN_PORT :: too many\n");
		return FALSE;
	}

#ifdef POK_CONFIG_PARTITIONS_PORTS
//(void) partition;
//return TRUE;
//if ((((uint8_t[]) POK_CONFIG_PARTITIONS_PORTS)[port]) == partition)
	if (partition_ports[port] == partition)
	{
#if defined(POK_SD) || defined(POK_QD)
		printf ("OWN_PORT :: owns!\n");
#endif
		return TRUE;
	}
#endif
	printf ("OWN_PORT :: mismatch!");
	return FALSE;
}

#endif // if defined (POK_NEEDS_PORTS_SAMPLING) || defined (POK_NEEDS_PORTS_QUEUEING)
