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
 * \file	 include/middleware/port.h
 * \brief	Describe queueing and sampling ports structures
 */

#ifndef __POK_KERNEL_PORTS_H__
#define __POK_KERNEL_PORTS_H__

 #if (defined (POK_NEEDS_PORTS_QUEUEING)) || (defined (POK_NEEDS_PORTS_SAMPLING))

#include <types.h>
#include <errno.h>
#include <core/lockobj.h>
#include <middleware/token.h>
#include <middleware/sizes.h>

/* size of the pok_queue_data array */
#define POK_QUEUE_PORT_ARRAY_SIZE 524288

/* max size of every port */
#define POK_PORT_MAX_SIZE 8192


typedef enum
{
	POK_PORT_QUEUEING_DISCIPLINE_FIFO		= 1,
	POK_PORT_QUEUEING_DISCIPLINE_PRIORITY	= 2
} pok_port_queueing_disciplines_t;

typedef enum
{ 
	POK_PORT_DIRECTION_OUT	= 0,
	POK_PORT_DIRECTION_IN	= 1
} pok_port_directions_t;

typedef pok_queueing_discipline_t pok_port_queueing_discipline_t;

typedef enum
{
	POK_PORT_KIND_QUEUEING	= 1,
	POK_PORT_KIND_SAMPLING	= 2,
	POK_PORT_KIND_INVALID	= 10
} pok_port_kinds_t;

typedef struct
{
	pok_port_id_t		identifier;
	pok_partition_id_t	partition;
	pok_port_size_t		index;
	bool_t				full;
	pok_port_size_t		size;  /* size of the queue allocate for the message: in case of queuing port: max_msg_size * max_nb_of_messages */
	pok_port_size_t		off_b; /* Offset of the beginning of the buffer */
	pok_port_size_t		off_e; /* Offset of the end of the buffer */
	pok_port_direction_t	direction;
	pok_port_queueing_discipline_t	discipline;
	pok_bool_t			ready;
	bool_t				empty;
	uint8_t				kind;
	uint64_t			refresh;
	uint64_t			last_receive;
	pok_lockobj_t		lock;
	bool_t				must_be_flushed;
	uint32_t			max_message_size;
#if (defined (POK_NEEDS_ARINC653)) 
 #ifdef POK_NEEDS_PORTS_QUEUEING
	uint32_t			msg_sizes[POK_CONFIG_MAX_QUEUING_MESSAGES];
	int8_t				first_size;
	uint16_t			nb_messages;								// number of messages present in the port
	uint16_t			max_nb_messages;
	pok_token_t 		tokens[POK_CONFIG_MAX_QUEUING_MESSAGES];	// Alternative PORT handling (see D2.2)
	int8_t				first_empty;
	int8_t				first_not_empty;
	bool_t				all_tokens_empty;
	bool_t				all_tokens_full;
 #endif
 #ifdef POK_NEEDS_PORTS_SAMPLING
	pok_token_t			token;	// Alternative PORT handling (see D2.2)
 #endif	
#endif	/* end if (defined (POK_NEEDS_ARINC653)) */
} pok_port_t;

#ifdef POK_NEEDS_PORTS_QUEUEING

typedef struct
{
	pok_port_size_t			size;
	pok_port_direction_t	direction;
	uint8_t					waiting_processes;
}pok_port_queueing_status_t;

/* Queueing port functions */
pok_ret_t pok_port_queueing_create ( char*			name, 
					const pok_port_size_t			size, 
					const pok_range_t  				max_nb_messages,
					const pok_port_direction_t		direction, 
					const pok_port_queueing_discipline_t	discipline,
					pok_port_id_t*					id,
					void*							partition_level_buffer);

pok_ret_t pok_port_queueing_receive (	const pok_port_id_t		id, 
					uint64_t				timeout, 
					const pok_port_size_t	maxlen, 
					void*					data, 
					pok_port_size_t*		len);

pok_ret_t pok_port_queueing_send (	const pok_port_id_t			id, 
					const void*				data, 
					const pok_port_size_t	len, 
					uint64_t				timeout);
/*
NOT USED at the moment
pok_ret_t pok_port_queueing_status (	const pok_port_id_t			id,
					pok_port_queueing_status_t*		status);

*/
pok_ret_t pok_port_queueing_id	  (	char*					name,
					pok_port_id_t*			id);
#endif /* end ifdef POK_NEEDS_PORTS_QUEUEING */


#ifdef POK_NEEDS_PORTS_SAMPLING

typedef struct
{
	pok_port_size_t			size;
	pok_port_direction_t	direction;
	uint64_t				refresh;
	bool_t					validity;
}pok_port_sampling_status_t;

/* Sampling port functions */
pok_ret_t pok_port_sampling_create (	char*					name,
					const pok_port_size_t			max_message_size,
					const pok_port_direction_t		direction, 
					const uint64_t					refresh,
					pok_port_id_t*					id,
					void*							partition_level_buffer);

pok_ret_t pok_port_sampling_write (	const pok_port_id_t			id,
					const void*			  		data,
					const pok_port_size_t		len);


pok_ret_t pok_port_sampling_read (	const pok_port_id_t			id,
					void*					message,
					pok_port_size_t*		len,
					bool_t*					valid);


pok_ret_t pok_port_sampling_id	 (	char*		name,
					pok_port_id_t*				id);

/*
NOT USED at the moment
pok_ret_t pok_port_sampling_status (	const pok_port_id_t			id,
					pok_port_sampling_status_t*		status);
*/
#endif

/* Generic functions */
pok_ret_t pok_port_create	(	char*			name, 
					const pok_port_size_t		size, 
					const pok_port_direction_t	direction,
					uint8_t						kind,
					pok_port_id_t*				id);


pok_ret_t pok_port_transfer (	const uint8_t 				gid_dst, 
					const uint8_t 			gid_src);

void 		pok_port_init(void);

pok_port_size_t	pok_port_available_size (uint8_t gid);

pok_port_size_t	pok_port_consumed_size 	(uint8_t gid);

pok_ret_t	pok_port_get (const uint8_t gid, void *data, const pok_port_size_t size);

pok_ret_t 	pok_port_get_message (const uint8_t pid, void *data,  pok_port_size_t * size);

pok_ret_t	pok_port_write (const uint8_t gid, const void *data, const pok_port_size_t size);

bool_t		pok_own_port (const uint8_t partition, const uint8_t port);

#endif /* end  #if (defined (POK_NEEDS_PORTS_QUEUEING)) || (defined (POK_NEEDS_PORTS_SAMPLING)) */

#endif
