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


#ifdef POK_NEEDS_DEBUG

#include <arch.h>
#include <errno.h>
#include <core/debug.h>
#include <core/cons.h>
#include <core/sched.h>
#include <core/thread.h>
#include <core/partition.h>


int debug_strlen (const char* str)
{
	int i = 0;

	while (*str != '\0')
	{
		str++;
		i++;
  	}
  return i;
}


void pok_debug_print_current_state ()
{
	uint32_t i;
	printf ("\nCurrent state\n");
	printf ("-------------\n");
	printf ("Kernel thread		: %d\n", KERNEL_THREAD);
	printf ("Idle thread		: %d\n", IDLE_THREAD);
#ifdef POK_NEEDS_PARTITIONS
	printf ("Current partition	: %d\n", POK_SCHED_CURRENT_PARTITION);
	printf ("Thread index		: %d\n", POK_CURRENT_PARTITION.thread_index);
	printf ("Thread low		: %d\n", POK_CURRENT_PARTITION.thread_index_low);
	printf ("Thread high		: %d\n", POK_CURRENT_PARTITION.thread_index_high);
	printf ("Thread capacity	: %d\n", POK_CURRENT_PARTITION.nthreads);
	printf ("Base addr		: 0x%x\n", POK_CURRENT_PARTITION.base_addr);
	printf ("Base vaddr		: 0x%x\n", POK_CURRENT_PARTITION.base_vaddr);
	printf ("Size			: %d\n", POK_CURRENT_PARTITION.size);
	printf ("Current thread		: %d\n", POK_CURRENT_PARTITION.current_thread);
	printf ("Prev current thread	: %d\n", POK_CURRENT_PARTITION.prev_current_thread);
	printf ("Main thread		: %d\n", POK_CURRENT_PARTITION.thread_main);
	printf ("Main thread entry	: 0x%x\n", POK_CURRENT_PARTITION.thread_main_entry);
	printf ("Partition threads sp 	:");
	for (i = POK_CURRENT_PARTITION.thread_index_low ; i < POK_CURRENT_PARTITION.thread_index_low + POK_CURRENT_PARTITION.thread_index ; i++)
	{
		printf (" 0x%x", pok_threads[i].sp);
	}
	printf ("\n");
	printf ("-------------\n");
#endif
	printf ("Current thread		: %d\n", POK_SCHED_CURRENT_THREAD);
	printf ("Period			: %d\n", POK_CURRENT_THREAD.period);
	printf ("Deadline		: %d\n", POK_CURRENT_THREAD.deadline);
	printf ("Partition		: %d\n", POK_CURRENT_THREAD.partition);
	printf ("sp			: 0x%x\n", POK_CURRENT_THREAD.sp);
	printf ("init_stack_addr	: 0x%x\n", POK_CURRENT_THREAD.init_stack_addr);
	printf ("entry			: 0x%x\n", POK_CURRENT_THREAD.entry);
}


void pok_fatal (const char* message)
{
  pok_write ("FATAL ERROR: \n", 13);
  pok_write (message , debug_strlen(message));
  POK_DEBUG_PRINT_CURRENT_STATE
  pok_arch_idle ();
}

#endif /* POK_CONFIG_NEEDS_DEBUG */
