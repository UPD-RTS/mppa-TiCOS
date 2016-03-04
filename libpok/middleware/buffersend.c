/*
 * Copyright (c) 2012  University of Padua, Department of Mathematics
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
 * This file incorporates work covered by the following copyright and
 * permission notice:
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

#include <core/dependencies.h>

#ifdef POK_NEEDS_BUFFERS

#include <errno.h>
#include <types.h>
#include <core/event.h>
#include <core/time.h>
#include <libc/string.h>
#include <middleware/buffer.h>

extern pok_buffer_t	pok_buffers[POK_CONFIG_NB_BUFFERS];
extern char			pok_buffers_data[MAX_BUFFERS_DATA_SIZE];

//#define DEBUG_BF 1

pok_ret_t pok_buffer_send (	const pok_buffer_id_t	id, 
				const void*		data, 
				const pok_port_size_t	len, 
				const uint64_t		timeout)
{
	pok_ret_t	ret;

	if (id > POK_CONFIG_NB_BUFFERS)
	{
#ifdef DEBUG_BF
		printf ("\tpok_buffer_send: NB\n");
#endif
		return POK_ERRNO_EINVAL;
	}

	if (pok_buffers[id].ready == FALSE)
	{
#ifdef DEBUG_BF
		printf ("\tpok_buffer_send: not ready\n");
#endif
		return POK_ERRNO_EINVAL;
	}

	if (data == NULL)
	{
#ifdef DEBUG_BF
		printf ("\tpok_buffer_send: null data\n");
#endif
		return POK_ERRNO_EINVAL;
	}
#ifdef DEBUG_BF
	else
	{
		printf ("\t data to write: 0x%x\n", ((unsigned char *)data)[0]);
		printf ("\t data to write: 0x%x\n", ((unsigned char *)data)[1]);
		printf ("\t data to write: 0x%x\n", ((unsigned char *)data)[2]);
		printf ("\t data to write: 0x%x\n", ((unsigned char *)data)[3]);
	}	
#endif

	if (len <= 0)
	{
#ifdef DEBUG_BF
		printf ("\tpok_buffer_send: length<0\n");
#endif
		return POK_ERRNO_EINVAL; 
	}
	if (len > pok_buffers[id].status.max_message_size) 
	{
#ifdef DEBUG_BF
		printf ("\tpok_buffer_send: length >>\n");
#endif
		return POK_ERRNO_EINVAL;
	}

#ifndef POK_NEEDS_SCHED_O1_SPLIT

	//pok_event_lock (pok_buffers[id].lock);

	while (pok_buffers[id].full)
	{
		if (timeout == 0)
		{
			//pok_event_unlock (pok_buffers[id].lock);
#ifdef DEBUG_BF
	printf ("\tpok_buffer_send: FULL >>\n");
#endif
			return POK_ERRNO_FULL;
		}
		else
		{
			// set the event room-in-the-buffer DOWN and waits for the it
			pok_event_lock (pok_buffers[id].lock);
			ret = pok_event_wait (pok_buffers[id].lock, timeout);

			if (ret != POK_ERRNO_OK)
			{
				pok_event_unlock (pok_buffers[id].lock);
#ifdef DEBUG_BF
				printf ("\tpok_buffer_send: ERROR after WAIT >>\n");
#endif
				return ret;
			}
		}
	}

#else /* POK_NEEDS_SCHED_O1_SPLIT is defined */
	/* wait for space (and for predecessor if the calling thread is a successor) */
	ret = pok_event_wait (pok_buffers[id].not_full, timeout);
	if (ret != POK_ERRNO_OK)
	{
		//pok_event_unlock (pok_buffers[id].lock);
#ifdef DEBUG_BF
		printf ("\tpok_buffer_send: ERROR after WAIT >>\n");
#endif
		return ret;
	}
#endif

	pok_port_size_t tmp_size;
	pok_port_size_t tmp_size2;

  	if ((pok_buffers[id].off_e + len) > pok_buffers[id].size)
	{
		tmp_size = pok_buffers[id].size - pok_buffers[id].off_e;
		memcpy (&pok_buffers_data[pok_buffers[id].index + pok_buffers[id].off_e], data, tmp_size);

		tmp_size2 = len - tmp_size;
		memcpy (&pok_buffers_data[pok_buffers[id].index], data + tmp_size, tmp_size2);
	}
	else
	{
		memcpy (&pok_buffers_data[pok_buffers[id].index + pok_buffers[id].off_e], data, len);
	}

	pok_buffers[id].off_e = (pok_buffers[id].off_e + len) % pok_buffers[id].size;

	if (pok_buffers[id].off_e == pok_buffers[id].off_b)
	{
#ifdef POK_NEEDS_SCHED_O1_SPLIT
		/* buffer is now full, lock the not-full lock object */
		pok_event_lock(pok_buffers[id].not_full);
#endif
		pok_buffers[id].full = TRUE;
	}

	pok_buffers[id].empty = FALSE;

	if (pok_buffers[id].first_size <0)
	{
		pok_buffers[id].first_size = 0;
	}		
	pok_buffers[id].msg_sizes[pok_buffers[id].status.nb_messages] = len;
	pok_buffers[id].status.nb_messages++;

	//pok_event_unlock (pok_buffers[id].lock);
#ifndef POK_NEEDS_SCHED_O1_SPLIT
	pok_event_broadcast (pok_buffers[id].lock);
#else /* POK_NEEDS_SCHED_O1_SPLIT is defined */
	/* with the split solution we unlock only the first waiting process per write */
	pok_event_signal (pok_buffers[id].not_empty);
#endif
	return POK_ERRNO_OK;
}

#endif /* POK_NEEDS_BUFFERS */

