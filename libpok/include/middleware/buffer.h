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


#ifndef __POK_USER_BUFFER_H__
#define __POK_USER_BUFFER_H__

#ifdef POK_NEEDS_BUFFERS

#ifndef  POK_CONFIG_MAX_BUFFERS_MESSAGE_SIZE
 #define  POK_CONFIG_MAX_BUFFERS_MESSAGE_SIZE 256
#endif

#define MAX_BUFFERS_DATA_SIZE		POK_CONFIG_MAX_BUFFERS_MESSAGE_SIZE*POK_CONFIG_NB_BUFFERS*POK_CONFIG_MAX_BUFFERS_MESSAGES

#define POK_BUFFER_DISCIPLINE_FIFO	1
#define POK_BUFFER_DISCIPLINE_PRIORITY	2

#include <types.h>
#include <errno.h>

#include <core/lockobj.h>

typedef struct
{
	pok_range_t		nb_messages;
	pok_range_t		max_messages;
	pok_size_t		max_message_size;
	pok_range_t		waiting_processes;
}pok_buffer_status_t;

typedef struct
{
	pok_bool_t			ready;
	pok_bool_t			empty;
	pok_bool_t			full;
	pok_size_t			size;	// pok_buffers[id].status.max_messages * pok_buffers[id].status.max_message_size
	pok_size_t			index;
	pok_port_size_t		off_b;
	pok_port_size_t		off_e;
	uint32_t			msg_sizes[POK_CONFIG_MAX_BUFFERS_MESSAGES];
	int8_t				first_size;
	pok_buffer_status_t	status;
	pok_queueing_discipline_t	discipline;
	pok_event_id_t				lock;
}pok_buffer_t;


pok_ret_t pok_buffer_create (char*							name, 
							const pok_range_t				max_nb_messages, 
							const pok_port_size_t			msg_size, 
							const pok_queueing_discipline_t	discipline,
							pok_buffer_id_t*				id);

pok_ret_t pok_buffer_receive (const pok_buffer_id_t			id, 
								const uint64_t		timeout, 
								void*				data, 
								pok_port_size_t*	len);

pok_ret_t pok_buffer_send (const pok_buffer_id_t			id, 
							const void*				data, 
							const pok_port_size_t	len, 
							const uint64_t			timeout);

pok_ret_t pok_port_buffer_status (const pok_buffer_id_t		id,
							const pok_buffer_status_t*	status);

pok_ret_t pok_buffer_id (char*				name,
						pok_buffer_id_t*	id);

#endif

#endif
