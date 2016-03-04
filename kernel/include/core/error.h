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



#ifndef __POK_CORE_ERROR_H__
#define __POK_CORE_ERROR_H__

#include <types.h>
#include <core/sched.h>

#define POK_ERROR_KIND_INVALID				9
#define POK_ERROR_KIND_DEADLINE_MISSED		10
#define POK_ERROR_KIND_APPLICATION_ERROR	11
#define POK_ERROR_KIND_NUMERIC_ERROR		12
#define POK_ERROR_KIND_ILLEGAL_REQUEST		13
#define POK_ERROR_KIND_STACK_OVERFLOW		14
#define POK_ERROR_KIND_MEMORY_VIOLATION		15
#define POK_ERROR_KIND_HARDWARE_FAULT		16
#define POK_ERROR_KIND_POWER_FAIL			17
#define POK_ERROR_KIND_PARTITION_CONFIGURATION  30 
#define POK_ERROR_KIND_PARTITION_INIT		31
#define POK_ERROR_KIND_PARTITION_SCHEDULING	32
#define POK_ERROR_KIND_PARTITION_HANDLER	33
#define POK_ERROR_KIND_PARTITION_PROCESS	34
#define POK_ERROR_KIND_KERNEL_INIT			50
#define POK_ERROR_KIND_KERNEL_SCHEDULING	51
#define POK_ERROR_KIND_KERNEL_CONFIG		52

#define POK_ERROR_MAX_MSG_SIZE				250

typedef struct
{
	uint8_t		error_kind;
	uint32_t	failed_thread;
	uint32_t	failed_addr;
	char		msg[POK_ERROR_MAX_MSG_SIZE];
	uint32_t	msg_size;
} pok_error_status_t;


#ifdef POK_NEEDS_ERROR_HANDLING

void		pok_error_declare (const uint8_t error);
pok_ret_t	pok_error_thread_create (uint32_t stack_size, void* entry);
void		pok_partition_error (uint8_t partition, uint32_t error);
void		pok_kernel_error (uint32_t error);

pok_ret_t	pok_error_handler_create (void* entry, uint32_t stack_size);


void 	pok_error_raise_application_error (
# if defined (POK_NEEDS_ARINC653)
	uint8_t error_code,
# endif
	char* msg, uint32_t msg_size);


#define POK_ERROR_CURRENT_PARTITION(error) pok_partition_error(pok_current_partition, error);

#endif /* ifdef POK_NEEDS_ERROR_HANDLING */

#endif
