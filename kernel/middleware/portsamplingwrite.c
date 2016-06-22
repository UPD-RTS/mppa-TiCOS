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
 * \file	 middleware/portsamplingwrite.c
 * \brief	Send data on a sampling port
 */

#ifdef POK_NEEDS_PORTS_SAMPLING

#include "dependencies.h"
#include <errno.h>
#include <types.h>
#include <libc.h>
#include <core/debug.h>
#include <core/sched.h>
#include <core/lockobj.h>
#include <middleware/port.h>
#include <middleware/queue.h>
#include <core/token.h>
#include <core/time.h>

extern uint8_t		pok_current_partition;
extern uint8_t		pok_ports_parts_nb_ports[POK_CONFIG_NB_PARTITIONS];
extern uint8_t*		pok_ports_parts_ports_identifiers[POK_CONFIG_NB_PARTITIONS];
extern uint8_t*		pok_ports_local_to_global[POK_CONFIG_NB_PARTITIONS];
extern pok_port_t	pok_ports[POK_CONFIG_NB_PORTS];
extern pok_queue_t	pok_queues[POK_CONFIG_NB_PARTITIONS];


pok_ret_t pok_port_sampling_write (	const pok_port_id_t	id,
					const void*		data,
					const pok_port_size_t	len)
{
 #ifdef POK_SD
	 printf ("Sampling port write");
 #endif
	/* We don't handle the timeout at this time */

	if (data == POK_NULL)
	{
 #ifdef POK_SD
			 printf ("NULL DATA!!");
 #endif
		return POK_ERRNO_EINVAL;
	}

	if (id > POK_CONFIG_NB_PORTS)
	{
 #ifdef POK_SD
		printf ("Port id out of range: %i", id);
 #endif
		return POK_ERRNO_EINVAL;
	}

	if (! pok_own_port (POK_SCHED_CURRENT_PARTITION, id))
	{
		return POK_ERRNO_PORT;
	}

	if (pok_ports[id].ready != TRUE)
	{
 #ifdef POK_SD
		printf ("\nPort NOT READY\n");
 #endif
		return POK_ERRNO_EINVAL;
	}

	if ((len <= 0 ) || (len > pok_ports[id].max_message_size))
	{
 #ifdef POK_SD
		printf ("\nPort SIZE (len:%i, size:%i)\n", len, pok_ports[id].max_message_size);
 #endif
		return POK_ERRNO_EINVAL;
	}

	if (pok_ports[id].direction != POK_PORT_DIRECTION_OUT)
	{
		return POK_ERRNO_DIRECTION;
	}


// Update the 'slot' with a reference to the data to be written (and size)
	pok_ports[id].token.empty = FALSE;
	pok_ports[id].token.msg_slot = (unsigned char *)data;
	pok_ports[id].token.size = len;
	pok_ports[id].off_e = len;
 #ifdef POK_SD
/*	printf ("\nWrite token P:%i msg '%c' or '%i' ( size:%i) address:0x%x\n", id,
	*(unsigned char*)pok_ports[id].token.msg_slot, *(int*)pok_ports[id].token.msg_slot,
	pok_ports[id].token.size, (uint32_t)pok_ports[id].token.msg_slot);
*/
 #endif
	pok_ports[id].last_receive = POK_GETTICK ();

 #ifdef POK_SD
	printf ("Written message: ");

	uint8_t i=0;
	for (i=0; i<len; i++)
	{
		printf("%c ",((char *)data)[i]);
	}

	printf (" in port: id %i ", id);
	printf (" (message length: %d)\n", len);
 #endif

	pok_ports[id].must_be_flushed = TRUE;

	return POK_ERRNO_OK;
}

#endif // POK_NEEDS_PORTS_SAMPLING
