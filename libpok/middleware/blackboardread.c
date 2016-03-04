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

#ifdef POK_NEEDS_BLACKBOARDS
#include <errno.h>
#include <types.h>
#include <core/event.h>
#include <libc/string.h>
#include <middleware/blackboard.h>

extern pok_blackboard_t	pok_blackboards[POK_CONFIG_NB_BLACKBOARDS];
extern char				pok_blackboards_data[MAX_BLACKBOARDS_DATA_SIZE];



pok_ret_t pok_blackboard_read (const pok_blackboard_id_t	id, 
							 const uint64_t					timeout,
							 void*							data,
							 pok_port_size_t*				len)
{
	(void) timeout;

	pok_ret_t ret;

	if (id > POK_CONFIG_NB_BLACKBOARDS)
	{
		return POK_ERRNO_EINVAL;
	}

	if (data == NULL)
	{
		return POK_ERRNO_EINVAL;
	}

	if (pok_blackboards[id].ready != TRUE)
	{
		return POK_ERRNO_EINVAL;
	}

  #ifndef POK_NEEDS_SCHED_O1_SPLIT
	 //pok_event_lock (pok_blackboards[id].lock);
	while (pok_blackboards[id].empty == TRUE)
	{

	#ifdef DEBUG_BB
		printf("Blackboard empty\n");
	#endif
		if (timeout == 0)
		{
//			pok_event_unlock (pok_blackboards[id].lock);
			return POK_ERRNO_EMPTY;
		}
		else
			{
			// set the event message-in-the-blacboard DOWN and waits for it
			pok_event_lock (pok_blackboards[id].lock);
			ret = pok_event_wait (pok_blackboards[id].lock, timeout);
			
			if (ret != POK_ERRNO_OK)
			{
				pok_event_unlock (pok_blackboards[id].lock);
				return ret;
			}
		}
	}
  #else /* POK_NEEDS_SCHED_O1_SPLIT is defined */
	/* wait for the data (and for predecessor if the calling thread is a successor) */
	ret = pok_event_wait (pok_blackboards[id].lock, timeout);
	//printf("READ: AFTER WAIT\n");
	if (ret != POK_ERRNO_OK)
	{
		return ret;
	}
	/* when here there are data on the blackboard. If the calling thread is a successor this also means
	 * that the predecessor has already executed
	 */
  #endif /* end ifndef POK_NEEDS_SCHED_O1_SPLIT */
	memcpy (data, &pok_blackboards_data[pok_blackboards[id].index], pok_blackboards[id].msgsize);

	//pok_event_unlock (pok_blackboards[id].lock);

	*len = pok_blackboards[id].msgsize;

	return ret;
}

#endif
