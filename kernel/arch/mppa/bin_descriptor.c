/**
 * Copyright (C) 2013-2016 Kalray SA.
 *
 * All rights reserved.
 *
 * Author: Pierre Guironnet de Massas, pgmassas@kalray.eu
 *
 */

#include <types.h>
#include "mOS_vcore_u.h"

#ifdef __k1a__
#define TLB_INVAL_ENTRY_LOW  TLB_INVAL_ENTRY_LOW_K1A
#define TLB_INVAL_ENTRY_HIGH TLB_INVAL_ENTRY_HIGH_K1A
#define MOS_ETHERNET_TLB_ENTRY (0x3002000030000863ULL)
#else
#define TLB_INVAL_ENTRY_LOW  TLB_INVAL_ENTRY_LOW_K1B
#define TLB_INVAL_ENTRY_HIGH TLB_INVAL_ENTRY_HIGH_K1B
#define MOS_ETHERNET_TLB_ENTRY (0x0442000004400093ULL)
#endif


#ifdef __k1dp__
#undef MOS_ETHERNET_TLB_ENTRY
#define MOS_ETHERNET_TLB_ENTRY MOS_NULL_TLB_ENTRY
#endif

#define VIRT_MEM_BASE (0x00000000000000000ULL)
#ifdef TEXT_PAGINATION
#define VIRT_MEM_BASE (0x80000000000000000ULL)
#endif

extern int TLB_INVAL_ENTRY_LOW  __attribute__((weak));
extern int TLB_INVAL_ENTRY_HIGH __attribute__((weak));

extern int _bin_size __attribute__((weak));
extern int _bin_start_frame;
extern int _bin_end_frame;

extern int _scoreboard_offset;
extern int _scb_mem_frames_array_offset;
extern int _vstart;
extern int _vhook_rm __attribute__((weak));


#ifdef __k1io__
  extern int _ddr_frame_start;
  extern int _ddr_frame_end;
#endif

extern int MOS_RESERVED;
extern int BINARY_SIZE          __attribute__((weak));
extern int _LIBMPPA_DSM_CLIENT_PAGE_SIZE    __attribute__((weak));
extern int _MOS_SECURITY_LEVEL;


#if BSP_NB_CLUSTER_P == 5
#  define EXPLORER
#endif


#ifdef EXPLORER
#define _DDR_SIZE_P (512*1024*1024ULL)
#else
#define _DDR_SIZE_P (2*1024*1024*1024ULL)
#endif


#define ADDR_0(A)           (A - 1)
#define ADDR_1(A)           (ADDR_0(A) | (ADDR_0(A) >> 1))
#define ADDR_2(A)           (ADDR_1(A) | (ADDR_1(A) >> 2))
#define ADDR_3(A)           (ADDR_2(A) | (ADDR_2(A) >> 4))
#define ADDR_4(A)           (ADDR_3(A) | (ADDR_3(A) >> 8))
#define ADDR_5(A)           (ADDR_4(A) | (ADDR_4(A) >> 16))
#define MEM_SIZE_ALIGN(A)   (ADDR_5(A) + 1)

extern int __MPPA_BURN_TX;
extern int __MPPA_BURN_FDIR;

// APPLICATION REQUIREMENTS
volatile mOS_bin_desc_t bin_descriptor __attribute__((section (".binaries"),weak)) = {
#ifdef __k1io__
	//.pe_pool            = (0x1 << ((BSP_NB_RM_IO_P ))) - 1,
	.pe_pool            = (0x1),
#else
	// .pe_pool            = (0x1 << ((BSP_NB_PE_P & ~(0x3)))) - 1,
	.pe_pool            = (0x1),
#endif
	.smem_start_frame        = (int)&_bin_start_frame,
	.smem_end_frame          = (int)&_bin_end_frame,

#ifdef __k1io__
	.ddr_start_frame        = (int)&_ddr_frame_start,
	.ddr_end_frame          = (int)&_ddr_frame_end,
#else
	.ddr_start_frame        = 0,
	.ddr_end_frame          = 0,
#endif

	.tlb_small_size         = 0x1000,   // 4K
#ifdef __k1a__
	.tlb_big_size           = 0x4000,
#else
#ifdef __k1io__
	.tlb_big_size           = 0x10000,
#else
	.tlb_big_size           = 0x8000,
#endif
#endif
	.entry_point        = (uint32_t) & _vstart,
	.security_level     = (int) &_MOS_SECURITY_LEVEL,
	.scoreboard_offset  = ( int ) &(_scoreboard_offset),
	.ltlb               = {
#ifdef __k1a__
		{._dword        = (0x000000000000087b) | (((uint64_t)MEM_SIZE_ALIGN(BSP_CLUSTER_INTERNAL_MEM_SIZE_P)) << 31)},
#ifdef __k1io__
		{._dword        = (0x8000000080000077) | (((uint64_t) (_DDR_SIZE_P)) << 31)},
		{._dword        = MOS_ETHERNET_TLB_ENTRY                                                                        },
#else
		 {._dword        = MOS_NULL_TLB_ENTRY                                                                        },
		 {._dword        = MOS_NULL_TLB_ENTRY                                                                        },
#endif
#else
		/*	{._dword        = (0x00000400000000db) | (((uint64_t)MEM_SIZE_ALIGN(BSP_CLUSTER_INTERNAL_MEM_SIZE_P)) << 31)}, */
#ifdef __k1io__
		{._dword        = (0x80000000800000db) | (((uint64_t) (_DDR_SIZE_P)) << 31)},
		{._dword        = MOS_ETHERNET_TLB_ENTRY                                                                        },
#else
		{._dword        = MOS_NULL_TLB_ENTRY                                                                        },
		{._dword        = MOS_NULL_TLB_ENTRY                                                                        },
#endif
		{._dword        = MOS_NULL_TLB_ENTRY                                                                        },
#endif
		[ 3 ... (MOS_VC_NB_USER_LTLB - 1) ] = {
			._dword        = MOS_NULL_TLB_ENTRY
		},

	},
	.jtlb               = { /* 128x16k jtlb entries @ 0x0 */
#ifdef __k1a__
		[0 ... (MOS_VC_NB_USER_JTLB - 1) ] = { ._dword       = MOS_NULL_TLB_ENTRY   },
#else
#ifdef __k1dp__
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{ ._dword =  0x8c00000080dbULL } /* VA: 0x8000 PA: 0x8000 */,
		{ ._dword =  0x44400000400dbULL } /* VA: 0x40000 PA: 0x40000 */,
		{ ._dword =  0x9c00000090dbULL } /* VA: 0x9000 PA: 0x9000 */,
		{ ._dword =  0x4c400000480dbULL } /* VA: 0x48000 PA: 0x48000 */,
		{ ._dword =  0xac000000a0dbULL } /* VA: 0xa000 PA: 0xa000 */,
		{ ._dword =  0x54400000500dbULL } /* VA: 0x50000 PA: 0x50000 */,
		{ ._dword =  0xbc000000b0dbULL } /* VA: 0xb000 PA: 0xb000 */,
		{ ._dword =  0x5c400000580dbULL } /* VA: 0x58000 PA: 0x58000 */,
		{ ._dword =  0xcc000000c0dbULL } /* VA: 0xc000 PA: 0xc000 */,
		{ ._dword =  0x64400000600dbULL } /* VA: 0x60000 PA: 0x60000 */,
		{ ._dword =  0xdc000000d0dbULL } /* VA: 0xd000 PA: 0xd000 */,
		{ ._dword =  0x6c400000680dbULL } /* VA: 0x68000 PA: 0x68000 */,
		{ ._dword =  0xec000000e0dbULL } /* VA: 0xe000 PA: 0xe000 */,
		{ ._dword =  0x74400000700dbULL } /* VA: 0x70000 PA: 0x70000 */,
		{ ._dword =  0xfc000000f0dbULL } /* VA: 0xf000 PA: 0xf000 */,
		{ ._dword =  0x7c400000780dbULL } /* VA: 0x78000 PA: 0x78000 */,
		{ ._dword =  0x10c00000100dbULL } /* VA: 0x10000 PA: 0x10000 */,
		{ ._dword =  0x84400000800dbULL } /* VA: 0x80000 PA: 0x80000 */,
		{ ._dword =  0x11c00000110dbULL } /* VA: 0x11000 PA: 0x11000 */,
		{ ._dword =  0x8c400000880dbULL } /* VA: 0x88000 PA: 0x88000 */,
		{ ._dword =  0x12c00000120dbULL } /* VA: 0x12000 PA: 0x12000 */,
		{ ._dword =  0x94400000900dbULL } /* VA: 0x90000 PA: 0x90000 */,
		{ ._dword =  0x13c00000130dbULL } /* VA: 0x13000 PA: 0x13000 */,
		{ ._dword =  0x9c400000980dbULL } /* VA: 0x98000 PA: 0x98000 */,
		{ ._dword =  0x14c00000140dbULL } /* VA: 0x14000 PA: 0x14000 */,
		{ ._dword =  0xa4400000a00dbULL } /* VA: 0xa0000 PA: 0xa0000 */,
		{ ._dword =  0x15c00000150dbULL } /* VA: 0x15000 PA: 0x15000 */,
		{ ._dword =  0xac400000a80dbULL } /* VA: 0xa8000 PA: 0xa8000 */,
		{ ._dword =  0x16c00000160dbULL } /* VA: 0x16000 PA: 0x16000 */,
		{ ._dword =  0xb4400000b00dbULL } /* VA: 0xb0000 PA: 0xb0000 */,
		{ ._dword =  0x17c00000170dbULL } /* VA: 0x17000 PA: 0x17000 */,
		{ ._dword =  0xbc400000b80dbULL } /* VA: 0xb8000 PA: 0xb8000 */,
		{ ._dword =  0x18c00000180dbULL } /* VA: 0x18000 PA: 0x18000 */,
		{ ._dword =  0xc4400000c00dbULL } /* VA: 0xc0000 PA: 0xc0000 */,
		{ ._dword =  0x19c00000190dbULL } /* VA: 0x19000 PA: 0x19000 */,
		{ ._dword =  0xcc400000c80dbULL } /* VA: 0xc8000 PA: 0xc8000 */,
		{ ._dword =  0x1ac000001a0dbULL } /* VA: 0x1a000 PA: 0x1a000 */,
		{ ._dword =  0xd4400000d00dbULL } /* VA: 0xd0000 PA: 0xd0000 */,
		{ ._dword =  0x1bc000001b0dbULL } /* VA: 0x1b000 PA: 0x1b000 */,
		{ ._dword =  0xdc400000d80dbULL } /* VA: 0xd8000 PA: 0xd8000 */,
		{ ._dword =  0x1cc000001c0dbULL } /* VA: 0x1c000 PA: 0x1c000 */,
		{ ._dword =  0xe4400000e00dbULL } /* VA: 0xe0000 PA: 0xe0000 */,
		{ ._dword =  0x1dc000001d0dbULL } /* VA: 0x1d000 PA: 0x1d000 */,
		{ ._dword =  0xec400000e80dbULL } /* VA: 0xe8000 PA: 0xe8000 */,
		{ ._dword =  0x1ec000001e0dbULL } /* VA: 0x1e000 PA: 0x1e000 */,
		{ ._dword =  0xf4400000f00dbULL } /* VA: 0xf0000 PA: 0xf0000 */,
		{ ._dword =  0x1fc000001f0dbULL } /* VA: 0x1f000 PA: 0x1f000 */,
		{ ._dword =  0xfc400000f80dbULL } /* VA: 0xf8000 PA: 0xf8000 */,
		{ ._dword =  0x20c00000200dbULL } /* VA: 0x20000 PA: 0x20000 */,
		{ ._dword =  0x104400001000dbULL } /* VA: 0x100000 PA: 0x100000 */,
		{ ._dword =  0x21c00000210dbULL } /* VA: 0x21000 PA: 0x21000 */,
		{ ._dword =  0x10c400001080dbULL } /* VA: 0x108000 PA: 0x108000 */,
		{ ._dword =  0x22c00000220dbULL } /* VA: 0x22000 PA: 0x22000 */,
		{ ._dword =  0x114400001100dbULL } /* VA: 0x110000 PA: 0x110000 */,
		{ ._dword =  0x23c00000230dbULL } /* VA: 0x23000 PA: 0x23000 */,
		{ ._dword =  0x11c400001180dbULL } /* VA: 0x118000 PA: 0x118000 */,
		{ ._dword =  0x24c00000240dbULL } /* VA: 0x24000 PA: 0x24000 */,
		{ ._dword =  0x124400001200dbULL } /* VA: 0x120000 PA: 0x120000 */,
		{ ._dword =  0x25c00000250dbULL } /* VA: 0x25000 PA: 0x25000 */,
		{ ._dword =  0x12c400001280dbULL } /* VA: 0x128000 PA: 0x128000 */,
		{ ._dword =  0x26c00000260dbULL } /* VA: 0x26000 PA: 0x26000 */,
		{ ._dword =  0x134400001300dbULL } /* VA: 0x130000 PA: 0x130000 */,
		{ ._dword =  0x27c00000270dbULL } /* VA: 0x27000 PA: 0x27000 */,
		{ ._dword =  0x13c400001380dbULL } /* VA: 0x138000 PA: 0x138000 */,
		{ ._dword =  0x28c00000280dbULL } /* VA: 0x28000 PA: 0x28000 */,
		{ ._dword =  0x144400001400dbULL } /* VA: 0x140000 PA: 0x140000 */,
		{ ._dword =  0x29c00000290dbULL } /* VA: 0x29000 PA: 0x29000 */,
		{ ._dword =  0x14c400001480dbULL } /* VA: 0x148000 PA: 0x148000 */,
		{ ._dword =  0x2ac000002a0dbULL } /* VA: 0x2a000 PA: 0x2a000 */,
		{ ._dword =  0x154400001500dbULL } /* VA: 0x150000 PA: 0x150000 */,
		{ ._dword =  0x2bc000002b0dbULL } /* VA: 0x2b000 PA: 0x2b000 */,
		{ ._dword =  0x15c400001580dbULL } /* VA: 0x158000 PA: 0x158000 */,
		{ ._dword =  0x2cc000002c0dbULL } /* VA: 0x2c000 PA: 0x2c000 */,
		{ ._dword =  0x164400001600dbULL } /* VA: 0x160000 PA: 0x160000 */,
		{ ._dword =  0x2dc000002d0dbULL } /* VA: 0x2d000 PA: 0x2d000 */,
		{ ._dword =  0x16c400001680dbULL } /* VA: 0x168000 PA: 0x168000 */,
		{ ._dword =  0x2ec000002e0dbULL } /* VA: 0x2e000 PA: 0x2e000 */,
		{ ._dword =  0x174400001700dbULL } /* VA: 0x170000 PA: 0x170000 */,
		{ ._dword =  0x2fc000002f0dbULL } /* VA: 0x2f000 PA: 0x2f000 */,
		{ ._dword =  0x17c400001780dbULL } /* VA: 0x178000 PA: 0x178000 */,
		{ ._dword =  0x30c00000300dbULL } /* VA: 0x30000 PA: 0x30000 */,
		{ ._dword =  0x184400001800dbULL } /* VA: 0x180000 PA: 0x180000 */,
		{ ._dword =  0x31c00000310dbULL } /* VA: 0x31000 PA: 0x31000 */,
		{ ._dword =  0x18c400001880dbULL } /* VA: 0x188000 PA: 0x188000 */,
		{ ._dword =  0x32c00000320dbULL } /* VA: 0x32000 PA: 0x32000 */,
		{ ._dword =  0x194400001900dbULL } /* VA: 0x190000 PA: 0x190000 */,
		{ ._dword =  0x33c00000330dbULL } /* VA: 0x33000 PA: 0x33000 */,
		{ ._dword =  0x19c400001980dbULL } /* VA: 0x198000 PA: 0x198000 */,
		{ ._dword =  0x34c00000340dbULL } /* VA: 0x34000 PA: 0x34000 */,
		{ ._dword =  0x1a4400001a00dbULL } /* VA: 0x1a0000 PA: 0x1a0000 */,
		{ ._dword =  0x35c00000350dbULL } /* VA: 0x35000 PA: 0x35000 */,
		{ ._dword =  0x1ac400001a80dbULL } /* VA: 0x1a8000 PA: 0x1a8000 */,
		{ ._dword =  0x36c00000360dbULL } /* VA: 0x36000 PA: 0x36000 */,
		{ ._dword =  0x1b4400001b00dbULL } /* VA: 0x1b0000 PA: 0x1b0000 */,
		{ ._dword =  0x37c00000370dbULL } /* VA: 0x37000 PA: 0x37000 */,
		{ ._dword =  0x1bc400001b80dbULL } /* VA: 0x1b8000 PA: 0x1b8000 */,
		{ ._dword =  0x38c00000380dbULL } /* VA: 0x38000 PA: 0x38000 */,
		{ ._dword =  0x1c4400001c00dbULL } /* VA: 0x1c0000 PA: 0x1c0000 */,
		{ ._dword =  0x39c00000390dbULL } /* VA: 0x39000 PA: 0x39000 */,
		{ ._dword =  0x1cc400001c80dbULL } /* VA: 0x1c8000 PA: 0x1c8000 */,
		{ ._dword =  0x3ac000003a0dbULL } /* VA: 0x3a000 PA: 0x3a000 */,
		{ ._dword =  0x1d4400001d00dbULL } /* VA: 0x1d0000 PA: 0x1d0000 */,
		{ ._dword =  0x3bc000003b0dbULL } /* VA: 0x3b000 PA: 0x3b000 */,
		{ ._dword =  0x1dc400001d80dbULL } /* VA: 0x1d8000 PA: 0x1d8000 */,
		{ ._dword =  0x3cc000003c0dbULL } /* VA: 0x3c000 PA: 0x3c000 */,
		{ ._dword =  0x1e4400001e00dbULL } /* VA: 0x1e0000 PA: 0x1e0000 */,
		{ ._dword =  0x3dc000003d0dbULL } /* VA: 0x3d000 PA: 0x3d000 */,
		{ ._dword =  0x1ec400001e80dbULL } /* VA: 0x1e8000 PA: 0x1e8000 */,
		{ ._dword =  0x3ec000003e0dbULL } /* VA: 0x3e000 PA: 0x3e000 */,
		{ ._dword =  0x1f4400001f00dbULL } /* VA: 0x1f0000 PA: 0x1f0000 */,
		{ ._dword =  0x3fc000003f0dbULL } /* VA: 0x3f000 PA: 0x3f000 */,
		{ ._dword =  0x1fc400001f80dbULL } /* VA: 0x1f8000 PA: 0x1f8000 */,

#else
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{ ._dword = 0x48400000400dbULL } /* VA: 0x40000 PA: 0x40000 */,
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{ ._dword = 0x58400000500dbULL } /* VA: 0x50000 PA: 0x50000 */,
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{ ._dword = 0x68400000600dbULL } /* VA: 0x60000 PA: 0x60000 */,
		{._dword        = MOS_NULL_TLB_ENTRY   },
		{ ._dword = 0x78400000700dbULL } /* VA: 0x70000 PA: 0x70000 */,
		{ ._dword = 0x8c00000080dbULL } /* VA: 0x8000 PA: 0x8000 */,
		{ ._dword = 0x88400000800dbULL } /* VA: 0x80000 PA: 0x80000 */,
		{ ._dword = 0x9c00000090dbULL } /* VA: 0x9000 PA: 0x9000 */,
		{ ._dword = 0x98400000900dbULL } /* VA: 0x90000 PA: 0x90000 */,
		{ ._dword = 0xac000000a0dbULL } /* VA: 0xa000 PA: 0xa000 */,
		{ ._dword = 0xa8400000a00dbULL } /* VA: 0xa0000 PA: 0xa0000 */,
		{ ._dword = 0xbc000000b0dbULL } /* VA: 0xb000 PA: 0xb000 */,
		{ ._dword = 0xb8400000b00dbULL } /* VA: 0xb0000 PA: 0xb0000 */,
		{ ._dword = 0xcc000000c0dbULL } /* VA: 0xc000 PA: 0xc000 */,
		{ ._dword = 0xc8400000c00dbULL } /* VA: 0xc0000 PA: 0xc0000 */,
		{ ._dword = 0xdc000000d0dbULL } /* VA: 0xd000 PA: 0xd000 */,
		{ ._dword = 0xd8400000d00dbULL } /* VA: 0xd0000 PA: 0xd0000 */,
		{ ._dword = 0xec000000e0dbULL } /* VA: 0xe000 PA: 0xe000 */,
		{ ._dword = 0xe8400000e00dbULL } /* VA: 0xe0000 PA: 0xe0000 */,
		{ ._dword = 0xfc000000f0dbULL } /* VA: 0xf000 PA: 0xf000 */,
		{ ._dword = 0xf8400000f00dbULL } /* VA: 0xf0000 PA: 0xf0000 */,
		{ ._dword = 0x10c00000100dbULL } /* VA: 0x10000 PA: 0x10000 */,
		{ ._dword = 0x108400001000dbULL } /* VA: 0x100000 PA: 0x100000 */,
		{ ._dword = 0x11c00000110dbULL } /* VA: 0x11000 PA: 0x11000 */,
		{ ._dword = 0x118400001100dbULL } /* VA: 0x110000 PA: 0x110000 */,
		{ ._dword = 0x12c00000120dbULL } /* VA: 0x12000 PA: 0x12000 */,
		{ ._dword = 0x128400001200dbULL } /* VA: 0x120000 PA: 0x120000 */,
		{ ._dword = 0x13c00000130dbULL } /* VA: 0x13000 PA: 0x13000 */,
		{ ._dword = 0x138400001300dbULL } /* VA: 0x130000 PA: 0x130000 */,
		{ ._dword = 0x14c00000140dbULL } /* VA: 0x14000 PA: 0x14000 */,
		{ ._dword = 0x148400001400dbULL } /* VA: 0x140000 PA: 0x140000 */,
		{ ._dword = 0x15c00000150dbULL } /* VA: 0x15000 PA: 0x15000 */,
		{ ._dword = 0x158400001500dbULL } /* VA: 0x150000 PA: 0x150000 */,
		{ ._dword = 0x16c00000160dbULL } /* VA: 0x16000 PA: 0x16000 */,
		{ ._dword = 0x168400001600dbULL } /* VA: 0x160000 PA: 0x160000 */,
		{ ._dword = 0x17c00000170dbULL } /* VA: 0x17000 PA: 0x17000 */,
		{ ._dword = 0x178400001700dbULL } /* VA: 0x170000 PA: 0x170000 */,
		{ ._dword = 0x18c00000180dbULL } /* VA: 0x18000 PA: 0x18000 */,
		{ ._dword = 0x188400001800dbULL } /* VA: 0x180000 PA: 0x180000 */,
		{ ._dword = 0x19c00000190dbULL } /* VA: 0x19000 PA: 0x19000 */,
		{ ._dword = 0x198400001900dbULL } /* VA: 0x190000 PA: 0x190000 */,
		{ ._dword = 0x1ac000001a0dbULL } /* VA: 0x1a000 PA: 0x1a000 */,
		{ ._dword = 0x1a8400001a00dbULL } /* VA: 0x1a0000 PA: 0x1a0000 */,
		{ ._dword = 0x1bc000001b0dbULL } /* VA: 0x1b000 PA: 0x1b000 */,
		{ ._dword = 0x1b8400001b00dbULL } /* VA: 0x1b0000 PA: 0x1b0000 */,
		{ ._dword = 0x1cc000001c0dbULL } /* VA: 0x1c000 PA: 0x1c000 */,
		{ ._dword = 0x1c8400001c00dbULL } /* VA: 0x1c0000 PA: 0x1c0000 */,
		{ ._dword = 0x1dc000001d0dbULL } /* VA: 0x1d000 PA: 0x1d000 */,
		{ ._dword = 0x1d8400001d00dbULL } /* VA: 0x1d0000 PA: 0x1d0000 */,
		{ ._dword = 0x1ec000001e0dbULL } /* VA: 0x1e000 PA: 0x1e000 */,
		{ ._dword = 0x1e8400001e00dbULL } /* VA: 0x1e0000 PA: 0x1e0000 */,
		{ ._dword = 0x1fc000001f0dbULL } /* VA: 0x1f000 PA: 0x1f000 */,
		{ ._dword = 0x1f8400001f00dbULL } /* VA: 0x1f0000 PA: 0x1f0000 */,
		{ ._dword = 0x20c00000200dbULL } /* VA: 0x20000 PA: 0x20000 */,
		{ ._dword = 0x208400002000dbULL } /* VA: 0x200000 PA: 0x200000 */,
		{ ._dword = 0x21c00000210dbULL } /* VA: 0x21000 PA: 0x21000 */,
		{ ._dword = 0x218400002100dbULL } /* VA: 0x210000 PA: 0x210000 */,
		{ ._dword = 0x22c00000220dbULL } /* VA: 0x22000 PA: 0x22000 */,
		{ ._dword = 0x228400002200dbULL } /* VA: 0x220000 PA: 0x220000 */,
		{ ._dword = 0x23c00000230dbULL } /* VA: 0x23000 PA: 0x23000 */,
		{ ._dword = 0x238400002300dbULL } /* VA: 0x230000 PA: 0x230000 */,
		{ ._dword = 0x24c00000240dbULL } /* VA: 0x24000 PA: 0x24000 */,
		{ ._dword = 0x248400002400dbULL } /* VA: 0x240000 PA: 0x240000 */,
		{ ._dword = 0x25c00000250dbULL } /* VA: 0x25000 PA: 0x25000 */,
		{ ._dword = 0x258400002500dbULL } /* VA: 0x250000 PA: 0x250000 */,
		{ ._dword = 0x26c00000260dbULL } /* VA: 0x26000 PA: 0x26000 */,
		{ ._dword = 0x268400002600dbULL } /* VA: 0x260000 PA: 0x260000 */,
		{ ._dword = 0x27c00000270dbULL } /* VA: 0x27000 PA: 0x27000 */,
		{ ._dword = 0x278400002700dbULL } /* VA: 0x270000 PA: 0x270000 */,
		{ ._dword = 0x28c00000280dbULL } /* VA: 0x28000 PA: 0x28000 */,
		{ ._dword = 0x288400002800dbULL } /* VA: 0x280000 PA: 0x280000 */,
		{ ._dword = 0x29c00000290dbULL } /* VA: 0x29000 PA: 0x29000 */,
		{ ._dword = 0x298400002900dbULL } /* VA: 0x290000 PA: 0x290000 */,
		{ ._dword = 0x2ac000002a0dbULL } /* VA: 0x2a000 PA: 0x2a000 */,
		{ ._dword = 0x2a8400002a00dbULL } /* VA: 0x2a0000 PA: 0x2a0000 */,
		{ ._dword = 0x2bc000002b0dbULL } /* VA: 0x2b000 PA: 0x2b000 */,
		{ ._dword = 0x2b8400002b00dbULL } /* VA: 0x2b0000 PA: 0x2b0000 */,
		{ ._dword = 0x2cc000002c0dbULL } /* VA: 0x2c000 PA: 0x2c000 */,
		{ ._dword = 0x2c8400002c00dbULL } /* VA: 0x2c0000 PA: 0x2c0000 */,
		{ ._dword = 0x2dc000002d0dbULL } /* VA: 0x2d000 PA: 0x2d000 */,
		{ ._dword = 0x2d8400002d00dbULL } /* VA: 0x2d0000 PA: 0x2d0000 */,
		{ ._dword = 0x2ec000002e0dbULL } /* VA: 0x2e000 PA: 0x2e000 */,
		{ ._dword = 0x2e8400002e00dbULL } /* VA: 0x2e0000 PA: 0x2e0000 */,
		{ ._dword = 0x2fc000002f0dbULL } /* VA: 0x2f000 PA: 0x2f000 */,
		{ ._dword = 0x2f8400002f00dbULL } /* VA: 0x2f0000 PA: 0x2f0000 */,
		{ ._dword = 0x30c00000300dbULL } /* VA: 0x30000 PA: 0x30000 */,
		{ ._dword = 0x308400003000dbULL } /* VA: 0x300000 PA: 0x300000 */,
		{ ._dword = 0x31c00000310dbULL } /* VA: 0x31000 PA: 0x31000 */,
		{ ._dword = 0x318400003100dbULL } /* VA: 0x310000 PA: 0x310000 */,
		{ ._dword = 0x32c00000320dbULL } /* VA: 0x32000 PA: 0x32000 */,
		{ ._dword = 0x328400003200dbULL } /* VA: 0x320000 PA: 0x320000 */,
		{ ._dword = 0x33c00000330dbULL } /* VA: 0x33000 PA: 0x33000 */,
		{ ._dword = 0x338400003300dbULL } /* VA: 0x330000 PA: 0x330000 */,
		{ ._dword = 0x34c00000340dbULL } /* VA: 0x34000 PA: 0x34000 */,
		{ ._dword = 0x348400003400dbULL } /* VA: 0x340000 PA: 0x340000 */,
		{ ._dword = 0x35c00000350dbULL } /* VA: 0x35000 PA: 0x35000 */,
		{ ._dword = 0x358400003500dbULL } /* VA: 0x350000 PA: 0x350000 */,
		{ ._dword = 0x36c00000360dbULL } /* VA: 0x36000 PA: 0x36000 */,
		{ ._dword = 0x368400003600dbULL } /* VA: 0x360000 PA: 0x360000 */,
		{ ._dword = 0x37c00000370dbULL } /* VA: 0x37000 PA: 0x37000 */,
		{ ._dword = 0x378400003700dbULL } /* VA: 0x370000 PA: 0x370000 */,
		{ ._dword = 0x38c00000380dbULL } /* VA: 0x38000 PA: 0x38000 */,
		{ ._dword = 0x388400003800dbULL } /* VA: 0x380000 PA: 0x380000 */,
		{ ._dword = 0x39c00000390dbULL } /* VA: 0x39000 PA: 0x39000 */,
		{ ._dword = 0x398400003900dbULL } /* VA: 0x390000 PA: 0x390000 */,
		{ ._dword = 0x3ac000003a0dbULL } /* VA: 0x3a000 PA: 0x3a000 */,
		{ ._dword = 0x3a8400003a00dbULL } /* VA: 0x3a0000 PA: 0x3a0000 */,
		{ ._dword = 0x3bc000003b0dbULL } /* VA: 0x3b000 PA: 0x3b000 */,
		{ ._dword = 0x3b8400003b00dbULL } /* VA: 0x3b0000 PA: 0x3b0000 */,
		{ ._dword = 0x3cc000003c0dbULL } /* VA: 0x3c000 PA: 0x3c000 */,
		{ ._dword = 0x3c8400003c00dbULL } /* VA: 0x3c0000 PA: 0x3c0000 */,
		{ ._dword = 0x3dc000003d0dbULL } /* VA: 0x3d000 PA: 0x3d000 */,
		{ ._dword = 0x3d8400003d00dbULL } /* VA: 0x3d0000 PA: 0x3d0000 */,
		{ ._dword = 0x3ec000003e0dbULL } /* VA: 0x3e000 PA: 0x3e000 */,
		{ ._dword = 0x3e8400003e00dbULL } /* VA: 0x3e0000 PA: 0x3e0000 */,
		{ ._dword = 0x3fc000003f0dbULL } /* VA: 0x3f000 PA: 0x3f000 */,
		{ ._dword = 0x3f8400003f00dbULL } /* VA: 0x3f0000 PA: 0x3f0000 */,
#endif
#endif
	},
	.rx_pool    = { . interface [ 0 ... MOS_NB_DMA_MAX -1 ] = {  .array64_bit = { ~(0x0ULL),         ~(0x0ULL),~(0x0ULL), ~(0x0ULL)}}},
	.uc_pool    = { . interface [ 0 ... MOS_NB_DMA_MAX -1 ] = ~(0x0)},
	//.tx_pool    = { . interface [ 0 ... MOS_NB_DMA_MAX -1 ] = ((0x1 << MPPA_DNOC_TX_CHANNELS_NUMBER)-1) & 0x3F}, /* All tx from 0 to 6 exclusted */
	.tx_pool    = { . interface [ 0 ... MOS_NB_DMA_MAX -1 ] = ((0x1 << MPPA_DNOC_TX_CHANNELS_NUMBER)-1)}, /* All tx */
	.mb_pool    = { . interface [ 0 ... MOS_NB_DMA_MAX -1 ] = { .array64_bit = { ~(0x0ULL),         ~(0x0ULL)}}},
	.mb_tx_pool = { . interface [ 0 ... MOS_NB_DMA_MAX -1 ] = 0xF},
	.fdir_pool  = { . interface [ 0 ... MOS_NB_DMA_MAX -1 ] =  0x1F},  /* Only loopback dir */
#ifdef __k1dp__
	.burn_tx    = (int)&__MPPA_BURN_TX,
	.burn_fdir  = (int)&__MPPA_BURN_FDIR,
#else
	.burn_tx    = -1,
	.burn_fdir  = -1,
#endif
	.hook_rm 	= (void (*)(void*)) &_vhook_rm
};
// APPLICATION REQUIREMENTS

mOS_scoreboard_t scoreboard __attribute__((section (".scoreboard")));
