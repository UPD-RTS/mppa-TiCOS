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


#ifndef APEX_TYPES
#define APEX_TYPES

#include <types.h>

#define SYSTEM_LIMIT_NUMBER_OF_PARTITIONS		32	/* module scope */
#define SYSTEM_LIMIT_NUMBER_OF_MESSAGES			512  /* module scope */
#define SYSTEM_LIMIT_MESSAGE_SIZE				8192 /* module scope */
#define SYSTEM_LIMIT_NUMBER_OF_PROCESSES		128  /* partition scope */
#define SYSTEM_LIMIT_NUMBER_OF_SAMPLING_PORTS	512  /* partition scope */
#define SYSTEM_LIMIT_NUMBER_OF_QUEUING_PORTS 	512  /* partition scope */
#define SYSTEM_LIMIT_NUMBER_OF_BUFFERS		 	256  /* partition scope */
#define SYSTEM_LIMIT_NUMBER_OF_BLACKBOARDS	 	256  /* partition scope */
#define SYSTEM_LIMIT_NUMBER_OF_SEMAPHORES	 	256  /* partition scope */
#define SYSTEM_LIMIT_NUMBER_OF_EVENTS			256  /* partition scope */


/* Base APEX types		  */
/*  The actual size of these base types is system specific and the	*/
/*  sizes must match the sizes used by the implementation of the	*/
/*  underlying Operating System.									*/

typedef	unsigned char	APEX_BYTE;				/* 8-bit unsigned */
typedef	long			APEX_INTEGER;			/* 32-bit signed */
typedef	unsigned long	APEX_UNSIGNED;			/* 32-bit unsigned */
typedef	long long		APEX_LONG_INTEGER;		/* 64-bit signed */

typedef
enum {
	NO_ERROR		=  0,	 /*  request valid and operation performed	  */
	NO_ACTION		=  1,	 /*  status of system unaffected by request	 */
	NOT_AVAILABLE	=  2,	 /*  resource required by request unavailable  */
	INVALID_PARAM	=  3,	 /*  invalid parameter specified in request	 */
	INVALID_CONFIG  =  4,	 /*  parameter incompatible with configuration */
	INVALID_MODE	=  5,	 /*  request incompatible with current mode	 */
	TIMED_OUT		=  6	  /*  time-out tied up with request has expired */
} RETURN_CODE_TYPE;

typedef
	struct {							/* expressed in us */
		APEX_UNSIGNED	MSB_TIME;		/* Big ...	 */
		APEX_UNSIGNED	LSB_TIME;		/* ... Endian */
	} TIME_TYPE;


#define	MAX_NAME_LENGTH				  30
typedef	char			NAME_TYPE[MAX_NAME_LENGTH];
typedef	void			(* SYSTEM_ADDRESS_TYPE);
typedef	APEX_BYTE*		MESSAGE_ADDR_TYPE;
typedef	APEX_INTEGER	MESSAGE_SIZE_TYPE;
typedef	APEX_INTEGER	MESSAGE_RANGE_TYPE;
typedef	enum { SOURCE = 0, DESTINATION = 1 } PORT_DIRECTION_TYPE;
typedef	enum { FIFO = 0, PRIORITY = 1 } QUEUING_DISCIPLINE_TYPE;

//typedef	APEX_LONG_INTEGER	  SYSTEM_TIME_TYPE; /* 64-bit signed integer with a 1 nanosecond LSB */
typedef	APEX_UNSIGNED		SYSTEM_TIME_TYPE; /* expressed in 100 us */

#define  INFINITE_TIME_VALUE			{4294967295uL, 4294967295uL};
#define  INFINITE_SYSTEM_TIME_VALUE	 	4294967295uL			 /* (2**32)-1 */
#define APERIODIC_PROCESS				INFINITE_SYSTEM_TIME_VALUE

#endif
