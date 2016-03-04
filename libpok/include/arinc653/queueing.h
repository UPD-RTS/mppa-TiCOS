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

#ifdef POK_NEEDS_ARINC653_QUEUEING

#ifndef POK_NEEDS_ARINC653_PROCESS
#define POK_NEEDS_ARINC653_PROCESS
#endif

#include <arinc653/process.h>
#include <arinc653/types.h>

#ifndef APEX_QUEUING
#define APEX_QUEUING
#define	MAX_NUMBER_OF_QUEUING_PORTS	  SYSTEM_LIMIT_NUMBER_OF_QUEUING_PORTS
typedef	NAME_TYPE		  QUEUING_PORT_NAME_TYPE;
typedef	APEX_INTEGER	  QUEUING_PORT_ID_TYPE;

typedef struct {
	MESSAGE_RANGE_TYPE		NB_MESSAGE;
	MESSAGE_RANGE_TYPE		MAX_NB_MESSAGE;
	MESSAGE_SIZE_TYPE		MAX_MESSAGE_SIZE;
	PORT_DIRECTION_TYPE		PORT_DIRECTION;
	WAITING_RANGE_TYPE		WAITING_PROCESSES;
} QUEUING_PORT_STATUS_TYPE;


extern void CREATE_QUEUING_PORT
		(const QUEUING_PORT_NAME_TYPE	/* in */	QUEUING_PORT_NAME, 
		const MESSAGE_SIZE_TYPE			/* in */	MAX_MESSAGE_SIZE, 
		const MESSAGE_RANGE_TYPE  		/* in */	MAX_NB_MESSAGE, 
		const PORT_DIRECTION_TYPE		/* in */	PORT_DIRECTION, 
		const QUEUING_DISCIPLINE_TYPE	/* in */	QUEUING_DISCIPLINE, 
		QUEUING_PORT_ID_TYPE * const	/* out */	QUEUING_PORT_ID, 
		RETURN_CODE_TYPE * const  		/* out */	RETURN_CODE);


extern void SEND_QUEUING_MESSAGE
		(const QUEUING_PORT_ID_TYPE		/* in */	QUEUING_PORT_ID, 
		const MESSAGE_ADDR_TYPE 		/* in */	MESSAGE_ADDR, 
		const MESSAGE_SIZE_TYPE			/* in */	LENGTH, 
		const SYSTEM_TIME_TYPE	 		/* in */	TIME_OUT, 
		RETURN_CODE_TYPE * const  		/* out */	RETURN_CODE);
//#if (POK_CONFIG_PART_NB_INPUT_QUEUING_PORTS > 0)
extern void RECEIVE_QUEUING_MESSAGE
		(const QUEUING_PORT_ID_TYPE		/* in */	QUEUING_PORT_ID, 
		const SYSTEM_TIME_TYPE	 		/* in */	TIME_OUT, 
		MESSAGE_ADDR_TYPE 				/* in out */MESSAGE_ADDR, 
		MESSAGE_SIZE_TYPE * const		/* out */	LENGTH, 
		RETURN_CODE_TYPE * const  		/* out */	RETURN_CODE);
//#endif
extern void GET_QUEUING_PORT_ID
		(const QUEUING_PORT_NAME_TYPE	/* in */	QUEUING_PORT_NAME, 
		QUEUING_PORT_ID_TYPE * const	/* out */	QUEUING_PORT_ID, 
		RETURN_CODE_TYPE * const  		/* out */	RETURN_CODE);

extern void GET_QUEUING_PORT_STATUS
		(const QUEUING_PORT_ID_TYPE		 /* in */	QUEUING_PORT_ID, 
		QUEUING_PORT_STATUS_TYPE * const /* out */	QUEUING_PORT_STATUS, 
		RETURN_CODE_TYPE * const  		/* out */	RETURN_CODE);

#endif
#endif
