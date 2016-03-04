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
 * \file	 core/time.c
 */

#if defined (POK_NEEDS_TIME) || defined (POK_NEEDS_SCHED) || defined (POK_NEEDS_THREADS)

#include <bsp.h>
#include <types.h>
#include <errno.h>
#include <core/time.h>
#include <core/thread.h>
#include <core/sched.h>
#include <core/partition.h>

T_TIME_64 DIV64(	T_TIME_64	DVD, T_TIME_64 DVS); 


/**
 * A global variable that contains the number
 * of elapsed ticks since the beginning
 */
uint64_t pok_tick_counter = 0;

/**
 * \brief Init the timing service.
 *
 * Call BSP-defined function to initialize
 * the timing service.
 */
void pok_time_init (void)
{
	pok_tick_counter = 0;
	pok_bsp_time_init ();
}

/**
 * Get the current POK clock, and store it in \a clk_val
 * Returns POK_ERRNO_OK
 */
pok_ret_t pok_get_system_time (uint64_t* clk_val)
{
	*clk_val = POK_GETTICK();
	return POK_ERRNO_OK;
}

 #if defined (POK_NEEDS_ARINC653) 
/**
 * Get the value of the TB and TBU registers in \a clk_val
 * Returns POK_ERRNO_OK
 */
pok_ret_t pok_get_HD_clock (uint64_t* clk_val)
{
	unsigned long long time_tmp;
	 
  #ifdef POK_ARCH_PPC // the POK_ARCH_PPC is defined in the misc/mk/common-ppc.mk file 
	unsigned long long cur_time = get_ppc_tb();
  #endif
	 
	T_TIME_64 time_tmp_64;
	T_TIME_64 frequency;
	frequency.MSB = 0;
	frequency.LSB = POK_BUS_FREQ_MHZ;
	 
	time_tmp = cur_time <<POK_FREQ_SHIFT;
	time_tmp_64 = * (T_TIME_64 * ) &time_tmp;
	time_tmp_64 =  DIV64 (time_tmp_64,frequency  );
	
	*clk_val = *(uint64_t *) &time_tmp_64;
	return POK_ERRNO_OK;
}
 #endif // ARINC

#endif /* POK_NEEDS_... */
