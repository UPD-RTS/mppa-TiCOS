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


int __vbsp_get_router_id(unsigned int node_id) __attribute__((section(".locked_text")));
int __vbsp_get_cluster_offset_within_bsp_platform(unsigned int router_id) __attribute__ ((section(".locked_text")));

/*
 * \brief The macro locked_assert checks a condition. If the condition evaluates to false then a message is printed and the execution is stopped.
 * If the macro NDEBUG was defined at the moment locked_assert.h was last included then the macro locked_assert does not generate any code.
 * \param exp a condition to be evaluated at runtime.
 *
 */
void
locked_assert_func(const char*, int, const char*, const char*) __attribute__((__noreturn__, section(".locked_text")));

#ifdef NDEBUG
#  define locked_assert(exp) ((void)0)
#else
#  define locked_assert(exp) ((exp) ? (void)0 : locked_assert_func (__FILE__, __LINE__, __func__, # exp))
#endif

int
locked_puts(const char *str) __attribute__((section (".locked_text")));


extern int _VBSP_OVERRIDE_NB_PE __attribute__ ((weak));
/** Number of physical cores in the cluster */
#ifdef __k1io__
#define VBSP_NB_PE      (((int) &_VBSP_OVERRIDE_NB_PE) ? ((int) &_VBSP_OVERRIDE_NB_PE) : (BSP_NB_RM_IO ))
#else
#define VBSP_NB_PE      (((int) &_VBSP_OVERRIDE_NB_PE) ? ((int) &_VBSP_OVERRIDE_NB_PE) : (BSP_NB_PE & ~(0x3)))
#endif

#define VBSP_NB_PE_MAX  (MOS_NB_PE_MAX)
#define VBSP_NB_PE_P    (MOS_NB_PE_P)

/**
 * @brief  Context type for hypervisor VM
 */
struct ___k1_vcontext_t {
	__k1_context_t ctxt;
	long ssp;
	long sssp;
};

typedef struct ___k1_vcontext_t __k1_vcontext_t;


/** Interrupt sources exposex by the vbsp lib */
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

/**
 * \brief Check if an event sent by a set of precessors is present on a specific event line
 * \param mask Set of processors that may have sent the requested event
 * \param pe_line Event line id within the BSP_NB_PE_EV_SRC range
 * \return returns 1 if all the processors described in mask have a pending event for
 * the current processor on event line pe_line. Returns 0 if that is not the case.
 */
int
bsp_inter_pe_event_is_present(mOS_vcore_set_t mask, int pe_line) __attribute__((section (".locked_text")));

/**
 * \brief Wait until an event was sent by each processor of a set on a specific event line. Then, clear
 * the event line.
 * \param mask Set of processors on which we wait for an event
 * \param pe_line Event line id within the BSP_NB_PE_EV_SRC range on which we wait for an event.
 */
void
bsp_inter_pe_event_waitclear(mOS_vcore_set_t mask, int pe_line) __attribute__((section (".locked_text")));

/**
 * \brief Wait until an event was sent by any processor on a specific event line. Then, clear
 * the event line.
 * \param pe_line Event line id within the BSP_NB_PE_EV_SRC range on which we wait for an event.
 * \return Returns the processor set that have sent an event on the requested line.
 */
mOS_vcore_set_t
bsp_inter_pe_event_waitany_clear(int pe_line) __attribute__((section (".locked_text")));

/**
 * \brief Clear an event incomming from a set of processors on a specific event line.
 * \param mask Set of processors on which pe_line event line will be clearer.
 * \param pe_line Event line id within the BSP_NB_PE_EV_SRC range on which we wait for an event.
 */
void
bsp_inter_pe_event_clear(mOS_vcore_set_t mask, int pe_line) __attribute__((section (".locked_text")));


/**
 * \brief Notify an event to a set of processors on a specific event line.
 * \param mask Set of processors to be notified.
 * \param pe_line Event line id within the BSP_NB_PE_EV_SRC range to be notified.
 */
void
bsp_inter_pe_event_notify(mOS_vcore_set_t mask, int pe_line) __attribute__((section (".locked_text")));

/**
 * \brief Raise an interrupt to a set of processors on a specific interrupt line.
 * \param mask Set of processors to be notified.
 * \param pe_line Event line id within the BSP_NB_PE_EV_SRC range to be notified.
 */
void
bsp_inter_pe_interrupt_raise(mOS_vcore_set_t mask, int pe_line) __attribute__ ((section(".locked_text")));

/**
 * \brief Clear a specific interrupt line.
 * \return Returns The set of processors that have raised an intterrupt on the specified line
 */
mOS_vcore_set_t
bsp_inter_pe_interrupt_clear(int pe_line) __attribute__ ((section(".locked_text")));

/**
 * \brief Enable an interrupt line
 */
void
bsp_interrupt_enable(bsp_ev_src_e source) __attribute__ ((section(".locked_text")));

/**
 * \brief Disable an interrupt line
 */
void
bsp_interrupt_disable(bsp_ev_src_e source) __attribute__ ((section(".locked_text")));

/** pe-2-pe event line to be used for a barrier depending on processor mode */
enum bsp_barrier_line {
	BSP_BARRIER_USER        = 0,
	BSP_BARRIER_EXCEPTION   = 1,
	BSP_BARRIER_INTERRUPT   = 2,
	BSP_BARRIER_SYSCALL     = 3,
	BSP_BARRIER_MAX         = BSP_BARRIER_SYSCALL
};

/** All pe's fast barrier implementation.  */
typedef struct {
	int global_sense;
	union {
		int word;
		char array[4];
	}                       masters;
	enum bsp_barrier_line line;
}bsp_barrier_t;

/** Barrier initializer macro */
#define BSP_BARRIER_INITIALIZER(__barrier_line) {0, {0}, __barrier_line}

/** Interrupt handler prototype */
typedef void ( *it_handler_t)(int nb, __k1_vcontext_t *ctx);


/**
 * \brief Implements an intra-cluster barrier. It blocks until all the processors
 * of the cluster enters the same barrier.
 * \param *b A pointer to the barrier structure
 */
//void
//bsp_barrier(bsp_barrier_t *b)  __attribute__ ((section(".locked_text") ));

/**
 * \brief Implements an intra-cluster half barrier prologue.
 * \param *b A pointer to the barrier structure
 */
//void
//bsp_barrier_in(bsp_barrier_t *b)  __attribute__ ((section(".locked_text") ));

/**
 * \brief Implements an intra-cluster half barrier epilogue.
 * \param *b A pointer to the barrier structure
 */
//void
//bsp_barrier_out(bsp_barrier_t *b)  __attribute__ ((section(".locked_text") ));

/**
 * \brief Initialize a intra-cluster barrier.
 * \param *b A pointer to the barrier structure
 * \param line Event line to be used by the barrier. The line number depends
 * on the expected processor mode for the barrier invocation.
 */
//void
//bsp_barrier_init(bsp_barrier_t *b, enum bsp_barrier_line line)  __attribute__ ((section(".locked_text")));



/**
 * \brief Register an interrupt handler for a bsp interrupt source.
 * \param it_handler The interrupt handler
 * \param source Interrupt id associated with the provided handler.
 */
void
bsp_register_it(it_handler_t it_handler, bsp_ev_src_e source) __attribute__ ((section(".locked_text")));
extern int _bin_start_frame;

/**
 * \brief Return the registered interrupt handler
 * \param source Interrupt id associated with the provided handler.
 * \return Interrupt handler pointer
 */
it_handler_t
bsp_get_registerered_it(bsp_ev_src_e source);


/**
 * \brief Converts a virtual address into a physical address
 * \param virtual_address A virtual address.
 * \return A physical address
 */
uintptr_t
bsp_virtual_to_physical(uintptr_t virtual_address);

#ifndef __k1a__
/**
 * \brief When possible, sets page access rights
 * \param Start address A virtual address
 * \param End address A virtual address
 * \param A valid value for tlb_entry_t . _ . pa
 * \return A physical address
 */
void
bsp_set_page_access_rights(uintptr_t start_address, uintptr_t end_address, int rights);
#endif

#ifndef NDEBUG
/**
 * \brief Terminates execution of the current application
 */
void
__vk1_do_exception_abort(__k1_vcontext_t *);
#endif

#ifdef __k1a__
void
__vbsp_sync_timestamp_with_remote_init();

/**
 * \brief Synchronize 64-bits timestamp counter with spawner
 */
void
__vbsp_sync_timestamp_with_remote(int remote_node_id);
#endif

 /* Old vbsp_p.h init */

void
pe_it_dispatcher(__k1_vcontext_t *ctx) __attribute__((section (".locked_text")));

enum { EV_BSP_INIT = BSP_EVENT_BSP } ev_bsp_e;
void
bsp_handlers_init(void) __attribute__((section (".init_code")));
void
bsp_event_init() __attribute__((section (".init_code")));

void __user_do_int (int it_nb __attribute__((unused)),
__k1_vcontext_t * ctx) __attribute__((section (".locked_text")));

void
__vk1_do_exception(__k1_trap_cause_t cause,
	__k1_vcontext_t *ctx) __attribute__((weak, section(".locked_text")));

 /* Old vbsp_p.h end */

extern int VIRT_U_MEM_START __attribute__ (( weak ));
extern int VIRT_U_MEM_END __attribute__ (( weak ));
extern int VIRT_U_MEM_PAG __attribute__ (( weak ));
extern int DDR_START __attribute__ (( weak ));
extern int DDR_SIZE __attribute__ (( weak ));
#ifdef __k1a__
#define CLUSTER_PERIPH_START 0x70000000
#define CLUSTER_PERIPH_END 0x70100000
#define IO_PERIPH_START 0x30000000
#define IO_PERIPH_END 0x50100000
#else
#define CLUSTER_PERIPH_START 0x2000000
#define CLUSTER_PERIPH_END 0x2300000
#define IO_PERIPH_START 0x4000000
#define IO_PERIPH_END 0x4700000
#endif


#ifdef __cplusplus
}
#endif

#endif
