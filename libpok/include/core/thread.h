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


#ifndef __POK_THREAD_H__
#define __POK_THREAD_H__

#include <core/dependencies.h>

#ifdef POK_NEEDS_THREADS

#include <types.h>
#include <errno.h>
#include <core/syscall.h>

#define POK_THREAD_DEFAULT_PRIORITY 	42
#define POK_DEFAULT_STACK_SIZE			2048

typedef struct
{
	uint8_t		priority;
	void*		entry;
	uint64_t	period;
	uint64_t	deadline;
	uint64_t	time_capacity;
	uint32_t	stack_size;
} pok_thread_attr_t;

void		pok_thread_init (void);
pok_ret_t	pok_thread_create (uint32_t* thread_id, const pok_thread_attr_t* attr);
pok_ret_t	pok_thread_sleep (const pok_time_t ms);
// pok_ret_t	pok_thread_sleep_until (const pok_time_t ms);
pok_ret_t	pok_thread_lock ();
pok_ret_t	pok_thread_unlock (const uint32_t thread_id);
// pok_ret_t	pok_thread_yield ();
unsigned int	pok_thread_current (void);
void		pok_thread_start (void (*entry)(), uint32_t id);
void		pok_thread_switch (uint32_t elected_id);
pok_ret_t	pok_thread_wait_infinite ();
void		pok_thread_wrapper ();
// pok_ret_t	pok_thread_attr_init (pok_thread_attr_t* attr);
pok_ret_t	pok_thread_period ();
pok_ret_t	pok_thread_id (uint32_t* thread_id);
pok_ret_t	pok_thread_resume (uint32_t thread_id);
pok_ret_t	pok_thread_suspend_other (uint32_t thread_id);
void 		pok_thread_init (void);

// #define pok_thread_sleep_until(time) pok_syscall2(POK_SYSCALL_THREAD_SLEEP_UNTIL, (uint32_t)time, 0)

#define pok_thread_wait_infinite() pok_thread_suspend()

#define pok_thread_suspend() pok_syscall2(POK_SYSCALL_THREAD_SUSPEND, NULL, NULL)

#define pok_thread_stop_self() pok_syscall2(POK_SYSCALL_THREAD_STOPSELF, 0, 0)

#define pok_thread_stop(id) pok_syscall2(POK_SYSCALL_THREAD_STOP, id, NULL)

#endif /* __POK_NEEDS_THREADS */
#endif /* __POK_THREAD_H__ */
