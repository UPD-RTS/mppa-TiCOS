/*
 *  Copyright (C) 2013-2016 Kalray SA.
 *  All rights reserved.
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
 *              Copyright (c) 2007-2009 POK team
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

#ifndef _BSP_H_
#define _BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <mOS_vcore_u.h>

/** Event and interrupt line definitions */
#define BSP_EV_LINE 0
#define BSP_IT_LINE 1

/** vsbp provides a set of virtualized event lines to be used by client librairies */
#define BSP_NB_PE_EV_SRC 16

/** Virtualized event lines 0..3 reserved to vbsp lib usage*/
#define BSP_EVENT_BSP 0

/** Virtualized event lines 4..7 reserved to guest libOS usage*/
#define BSP_EVENT_LIBOS 4

/** Virtualized event lines 8..11 reserved to mppa_dsm_clientlib usage*/
#define BSP_EVENT_LIBMPPA_DSM_CLIENT 8

/** Virtualized event lines 12..15 reserved to user application usage*/
#define BSP_EVENT_USER 12

#ifndef BSP_CLUSTER_INTERNAL_MEM_SIZE_P
#  define BSP_CLUSTER_INTERNAL_MEM_SIZE_P (0x200000)
#endif

/** Number of timers exposed by the vbsp lib */
#define BSP_NB_TIMERS 2

/** Target section for data's */
#define TARGET_DATA ".locked_data"

/** Target section for text */
#define TARGET_TEXT ".locked_text"

#ifdef __k1dp__
/** Number of interrupt sources exposed by the vbsp lib */
#define BSP_NB_IT_SOURCES 24
#else
#define BSP_NB_IT_SOURCES 32
#endif

#define MOS_HAVE_VCONTEXT_T

/**
 * @brief  Context type for hypervisor VM
 */
struct ___k1_vcontext_t {
	__k1_context_t ctxt;
	long ssp;
	long sssp;
};

typedef struct ___k1_vcontext_t __k1_vcontext_t;

/**
 * @brief  Interrupt sources exposex by the vbsp lib
 */
typedef enum {
	BSP_IT_TIMER_0  = 0,    /**< timer 0 */
	BSP_IT_TIMER_1  = 1,    /**< timer 1 */
	BSP_IT_WDOG     = 2,    /**< watchdog */
	BSP_IT_CN       = 3,    /**< control noc */
	BSP_IT_RX       = 4,    /**< dnoc rx */
	BSP_IT_UC       = 5,    /**< dma uc  */
	BSP_IT_NE       = 6,    /**< noc error  */
	BSP_IT_WDOG_U   = 7,    /**< watchdog underflow */
	BSP_IT_PE_0     = 8,    /**< remote pe event 0 */
	BSP_IT_PE_1     = 9,    /**< remote pe event 1 */
	BSP_IT_PE_2     = 10,   /**< remote pe event 2 */
	BSP_IT_PE_3     = 11,   /**< remote pe event 3 */
	BSP_IT_PE_4     = 12,   /**< remote pe event 4 */
	BSP_IT_PE_5     = 13,   /**< remote pe event 5 */
	BSP_IT_PE_6     = 14,   /**< remote pe event 6 */
	BSP_IT_PE_7     = 15,   /**< remote pe event 7 */
	BSP_IT_PE_8     = 16,   /**< remote pe event 8 */
	BSP_IT_PE_9     = 17,   /**< remote pe event 9 */
	BSP_IT_PE_10    = 18,   /**< remote pe event 10 */
	BSP_IT_PE_11    = 19,   /**< remote pe event 11 */
	BSP_IT_PE_12    = 20,   /**< remote pe event 12 */
	BSP_IT_PE_13    = 21,   /**< remote pe event 13 */
	BSP_IT_PE_14    = 22,   /**< remote pe event 14 */
	BSP_IT_PE_15    = 23,   /**< remote pe event 15 */
#ifdef __k1io__
	BSP_IT_GIC_1,           /**< GiC 1 */
	BSP_IT_GIC_2,           /**< GiC 2 */
	BSP_IT_GIC_3,           /**< GiC 3 */

#endif
}bsp_ev_src_e;


/** Interrupt handler prototype */
typedef void ( *it_handler_t)(int nb, __k1_vcontext_t *ctx);


#ifdef __cplusplus
}
#endif

#endif
