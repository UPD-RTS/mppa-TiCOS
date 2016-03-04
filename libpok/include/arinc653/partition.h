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


#ifdef POK_NEEDS_ARINC653_PARTITION

#include <arinc653/types.h>
#include <arinc653/process.h>

#ifndef APEX_PARTITION
#define APEX_PARTITION
#define	MAX_NUMBER_OF_PARTITIONS	SYSTEM_LIMIT_NUMBER_OF_PARTITIONS
typedef enum
{
	IDLE		= 0,
	COLD_START	= 1,
	WARM_START	= 2,
	NORMAL	 	= 3
} OPERATING_MODE_TYPE;

typedef	APEX_INTEGER	  PARTITION_ID_TYPE;
typedef enum
{
	NORMAL_START			= 0,
	PARTITION_RESTART		= 1,
	HM_MODULE_RESTART		= 2,
	HM_PARTITION_RESTART 	= 3
} START_CONDITION_TYPE;

typedef struct {
	SYSTEM_TIME_TYPE	PERIOD;
	SYSTEM_TIME_TYPE	DURATION;
	PARTITION_ID_TYPE	IDENTIFIER;
	LOCK_LEVEL_TYPE		LOCK_LEVEL;
	OPERATING_MODE_TYPE OPERATING_MODE;
	// START_CONDITION_TYPE START_CONDITION;
} PARTITION_STATUS_TYPE;

extern void GET_PARTITION_STATUS
		(PARTITION_STATUS_TYPE * const	/* out */	PARTITION_STATUS, 
		RETURN_CODE_TYPE * const  		/* out */	RETURN_CODE);


extern void GET_PARTITION_START_CONDITION
		(START_CONDITION_TYPE * const	/* out */	START_CONDITION, 
		RETURN_CODE_TYPE * const  		/* out */	RETURN_CODE);

extern void SET_PARTITION_MODE
		(const OPERATING_MODE_TYPE		/* in */	OPERATING_MODE, 
		RETURN_CODE_TYPE * const  		/* out */	RETURN_CODE);

#endif

#endif
