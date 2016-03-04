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
#include <core/time.h>
#include <core/event.h>
#include <libc/string.h>
#include <middleware/buffer.h>

extern pok_buffer_t	pok_buffers[POK_CONFIG_NB_BUFFERS];
extern char*		pok_buffers_names[POK_CONFIG_NB_BUFFERS];
pok_size_t			pok_buffers_data_index = 0;
pok_bool_t			first_time = TRUE;

pok_ret_t pok_buffer_create (char*								name, 
							  const pok_range_t					max_nb_messages, 
							  const pok_port_size_t				max_msg_size, 
							  const pok_queueing_discipline_t	discipline,
							  pok_buffer_id_t*					id)
{
	uint8_t	  	n;
	pok_ret_t	ret;

	if (first_time)
	{
		 pok_buffer_init();
		 first_time = FALSE;
	}

	for (n=0 ; n < POK_CONFIG_NB_BUFFERS ; n++)
	{
		if (streq (name, pok_buffers_names[n]))
		{
			if (pok_buffers[n].ready == TRUE)
			{
				return POK_ERRNO_READY;
			}
#ifdef POK_NEEDS_SCHED_O1_SPLIT
	 		ret = pok_event_create (&pok_buffers[n].not_full);
			if (ret != POK_ERRNO_OK)
			{
		 		printf ("\tpok_buffer_create: Err in 'pok_event_create' for not_full event\n");
				return ret;
			}
			ret = pok_event_create (&pok_buffers[n].not_empty);
			if (ret != POK_ERRNO_OK)
			{
				printf ("\tpok_buffer_create: Err in 'pok_event_create' for not_empty event\n");
				return ret;
			}
			/* the not-empty has to be set DOWN because a new buffer is empy */
			pok_event_lock(pok_buffers[n].not_empty);
#else
			ret = pok_event_create (&pok_buffers[n].lock);

			if (ret != POK_ERRNO_OK)
			{
//						printf ("\tpok_buffer_create: Err in 'pok_event_create'\n");
				return ret;
			}
#endif
			pok_buffers[n].index			= pok_buffers_data_index;
			pok_buffers[n].ready			= TRUE;
			pok_buffers[n].empty			= TRUE;
			pok_buffers[n].size			 	= max_nb_messages * max_msg_size;
			pok_buffers[n].first_size		= -1;
			pok_buffers[n].off_e			= 0;
			pok_buffers[n].off_b			= 0;
			pok_buffers[n].discipline		= discipline;
			pok_buffers_data_index			= pok_buffers_data_index +  pok_buffers[n].size;
			pok_buffers[n].status.nb_messages 	= 0;
			pok_buffers[n].status.max_messages	= max_nb_messages;
			pok_buffers[n].status.max_message_size	=  max_msg_size;
			pok_buffers[n].status.waiting_processes = 0;

			*id = n;
			return POK_ERRNO_OK;
		}
	}
	printf ("\tpok_buffer_create: Name mismatch\n");
	return POK_ERRNO_EINVAL;
}

#endif /* POK_NEEDS_BUFFERS */


