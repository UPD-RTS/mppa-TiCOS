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


#if defined (POK_NEEDS_TIME) || defined (POK_NEEDS_THREADS) || defined (POK_NEEDS_SCHED)

#ifndef __POK_TIME_H__
#define __POK_TIME_H__

#include <types.h>
#include <errno.h>

#ifndef POK_BUS_FREQ_MHZ
#define POK_BUS_FREQ_MHZ 74
#endif

#define POK_BUS_FREQ_HZ ( POK_BUS_FREQ_MHZ * 1000000U)

#ifndef POK_FREQ_DIV
 #ifdef POK_NEEDS_QEMU_SETUP 
	#define POK_FREQ_DIV 1
 #else
	#define POK_FREQ_DIV 1
 #endif
#endif

#ifndef POK_FREQ_SHIFT
#define POK_FREQ_SHIFT 0
#endif


/**
 * The rate of the clock in POK
 */
#ifdef POK_NEEDS_QEMU_SETUP 
//	#define POK_TIMER_FREQUENCY 5
	#define POK_TIMER_FREQUENCY 1
#else
	#define POK_TIMER_FREQUENCY 1000
#endif
extern uint64_t pok_tick_counter;

//#define CLOCK_HANDLER pok_tick_counter += 1; pok_sched ();

#ifdef POK_NEEDS_SCHED_O1

 #ifdef POK_NEEDS_SCHED_O1_SPLIT
  #define POK_CLOCK() get_ppc_tb()
 #endif

extern uint64_t pok_update_tick();
 #define POK_GETTICK() pok_update_tick()

#else /* POK_NEEDS_SCHED_O1 is not defined */

 #define POK_GETTICK() pok_tick_counter

#endif /* end ifdef POK_NEEDS_SCHED_O1 */

typedef int clockid_t;

# define CLOCK_REALTIME 0

typedef long int time_t;

struct timespec
{
	 time_t tv_sec;				  /* Seconds.  */
	 long int tv_nsec;			  /* Nanoseconds.  */
};

typedef struct  
{
	 unsigned int MSB;
	 unsigned int LSB;
} T_TIME_64 ;


typedef enum
{
	__POK_CLOCK_REALTIME = 0 
} pok_clockid_t;

void		pok_time_init (void);
pok_ret_t	pok_get_system_time (uint64_t* clk_val);
pok_ret_t	pok_get_HD_clock (uint64_t* clk_val);


#ifdef POK_ARCH_PPC
inline unsigned long long get_ppc_tb (void);
#endif


#endif  /* __POK_TIME_H__ */
#endif /* Needs clauses */
