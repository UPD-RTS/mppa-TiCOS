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


#ifdef POK_NEEDS_ARINC653_PROCESS

#include <core/dependencies.h>
#include <core/thread.h>
#include <arinc653/types.h>
#include <arinc653/process.h>
#include <libc/string.h>
#include <libc/stdio.h>


typedef char					pok_arinc653_threads_name_t[MAX_NAME_LENGTH];
/* thread names */
pok_arinc653_threads_name_t 	pok_arinc653_threads_names[POK_CONFIG_NB_THREADS];


/********************************************************************
* SERVICE  GET_PROCESS_STATUS
*********************************************************************/
void GET_PROCESS_STATUS
		(const PROCESS_ID_TYPE	 	/* in */	PROCESS_ID, 
		PROCESS_STATUS_TYPE * const	/* out */	PROCESS_STATUS, 
		RETURN_CODE_TYPE * const  	/* out */	RETURN_CODE)
{

	PROCESS_NAME_TYPE	* name = (PROCESS_STATUS->ATTRIBUTES).NAME;

	strcpy(name, &(pok_arinc653_threads_names[PROCESS_ID])); 	

	(PROCESS_STATUS->ATTRIBUTES).ENTRY_POINT = 0;
	(PROCESS_STATUS->ATTRIBUTES).STACK_SIZE = 0;
	(PROCESS_STATUS->ATTRIBUTES).BASE_PRIORITY = FIFO;
	(PROCESS_STATUS->ATTRIBUTES).PERIOD = 0;
	(PROCESS_STATUS->ATTRIBUTES).TIME_CAPACITY = 0;
	(PROCESS_STATUS->ATTRIBUTES).DEADLINE = SOFT;

	PROCESS_STATUS->CURRENT_PRIORITY = 0;
	(PROCESS_STATUS->DEADLINE_TIME).MSB_TIME = 0;
	(PROCESS_STATUS->DEADLINE_TIME).LSB_TIME = 0;
	PROCESS_STATUS->PROCESS_STATE = DORMANT;

	*RETURN_CODE = NO_ERROR;

}

/********************************************************************
* SERVICE  GET_MY_ID
*********************************************************************/
void GET_MY_ID (PROCESS_ID_TYPE * const		/* out */	PROCESS_ID, 
		RETURN_CODE_TYPE * const	/* out */	RETURN_CODE)
{

	uint32_t	core_process_id;
	pok_thread_id	(&core_process_id);
	
	*PROCESS_ID 	= core_process_id;
	*RETURN_CODE 	= NO_ERROR;

}


/********************************************************************
* SERVICE  GET_PROCESS_ID
*********************************************************************/
void GET_PROCESS_ID
		(const PROCESS_NAME_TYPE	/* in */	PROCESS_NAME, 
		PROCESS_ID_TYPE * const		/* out */	PROCESS_ID, 
		RETURN_CODE_TYPE * const 	/* out */	RETURN_CODE)
{

	uint16_t i;

	bool_t  found = FALSE;

	for (i = 0 ; i < POK_CONFIG_NB_THREADS ; i++)
	{
		if (strncmp(pok_arinc653_threads_names[i],PROCESS_NAME, strlen(PROCESS_NAME)) == 0)
		{
			found=TRUE;
			*PROCESS_ID = i;
			*RETURN_CODE = NO_ERROR;
			 break;
		}
	}
}


/********************************************************************
* SERVICE  CREATE_PROCESS
********************************************************************/
void CREATE_PROCESS
		(const PROCESS_ATTRIBUTE_TYPE * const /* Big */	/* in */ATTRIBUTE, 
		PROCESS_ID_TYPE * const	/* out */		 				PROCESS_ID, 
		RETURN_CODE_TYPE * const  /* out */		 				RETURN_CODE)
{
	pok_thread_attr_t	core_attr;
	pok_ret_t			core_ret;
	uint32_t			core_process_id = 0;

	core_attr.priority		= (uint8_t) ATTRIBUTE->BASE_PRIORITY;
	core_attr.entry			= ATTRIBUTE->ENTRY_POINT;
	core_attr.period		= ATTRIBUTE->PERIOD; 

	if ((core_attr.period) != INFINITE_SYSTEM_TIME_VALUE){
		core_attr.period	= ATTRIBUTE->PERIOD / 10; // expressed in 100 us, converted ms
	}

	core_attr.deadline = ATTRIBUTE->DEADLINE; 
	if ((core_attr.deadline) > 0){
		core_attr.deadline		  = ATTRIBUTE->DEADLINE / 10; 
	}

	core_attr.time_capacity	= ATTRIBUTE->TIME_CAPACITY;// expressed in 100 us, converted ms
	
	if ((core_attr.time_capacity) > 0){
		core_attr.time_capacity = ATTRIBUTE->TIME_CAPACITY / 10;
	}
	core_attr.stack_size		= ATTRIBUTE->STACK_SIZE;

	core_ret = pok_thread_create (&core_process_id, &core_attr);

	*(pok_arinc653_threads_names[core_process_id]) = '\0';

	strcpy(&(pok_arinc653_threads_names[core_process_id]),&(ATTRIBUTE->NAME));

	*PROCESS_ID = core_process_id;

	*RETURN_CODE = core_ret;
}



/********************************************************************
* SERVICE  STOP
********************************************************************/
void STOP (const PROCESS_ID_TYPE	 	/* in */	PROCESS_ID, 
		 RETURN_CODE_TYPE * const  	/* out */	RETURN_CODE){

	pok_ret_t		core_ret;

	core_ret		= pok_thread_stop (PROCESS_ID);
	*RETURN_CODE	= core_ret;
}



/********************************************************************
* SERVICE  START
********************************************************************/
void START
	(const PROCESS_ID_TYPE		/* in */	PROCESS_ID, 
	RETURN_CODE_TYPE * const	/* out */	RETURN_CODE){

	pok_ret_t			core_ret;
//	printf ("IN START...process id:  %d \n", PROCESS_ID);
	core_ret = pok_thread_libpok_start (PROCESS_ID);
	*RETURN_CODE =  core_ret;
}


void DELAYED_START (PROCESS_ID_TYPE	PROCESS_ID,
			 SYSTEM_TIME_TYPE  DELAY_TIME,
			 RETURN_CODE_TYPE  *RETURN_CODE )
{
	pok_ret_t		core_ret;
	core_ret 		= pok_thread_libpok_delayed_start (PROCESS_ID, DELAY_TIME);
	*RETURN_CODE 	= core_ret;
}

/********************************************************************
* SERVICE  STOP_SELF
*********************************************************************/
void STOP_SELF ()
{
	pok_thread_stop_self();
}

/********************************************************************
* SERVICE  SUSPEND
*********************************************************************/
void SUSPEND (PROCESS_ID_TYPE	 PROCESS_ID, RETURN_CODE_TYPE	*return_code )
{
		pok_ret_t		core_ret;

		core_ret		= pok_thread_suspend_other(PROCESS_ID);
		*return_code 	= NO_ERROR;
}

/********************************************************************
* SERVICE  SUSPEND_SELF
*********************************************************************/
void SUSPEND_SELF (SYSTEM_TIME_TYPE time_out, RETURN_CODE_TYPE *return_code )
{
		//pok_ret_t		core_ret;
		(void) time_out; // Time-out function has not been implemented yet
		//core_ret = 
	 pok_thread_suspend();
		*return_code = NO_ERROR;
}

/********************************************************************
* SERVICE  RESUME
*********************************************************************/
void RESUME (PROCESS_ID_TYPE  /* in*/	PROCESS_ID,
				 RETURN_CODE_TYPE /*out*/  *RETURN_CODE )
{
		pok_ret_t	core_ret;	
/* Implement required checks */
		core_ret = pok_thread_resume (PROCESS_ID);
		*RETURN_CODE =  core_ret;
}


#ifndef POK_CONFIG_OPTIMIZE_FOR_GENERATED_CODE
/********************************************************************
* SERVICE  SET_PRIORITY
*********************************************************************/
void SET_PRIORITY (PROCESS_ID_TYPE  process_id,
						 PRIORITY_TYPE	 priority,
						 RETURN_CODE_TYPE *return_code )
{
	(void) process_id;
	(void) priority;
	*return_code = NOT_AVAILABLE;
}


/********************************************************************
* SERVICE  LOCK_PREEMPTION
*********************************************************************/
void LOCK_PREEMPTION (LOCK_LEVEL_TYPE	  *lock_level,
							 RETURN_CODE_TYPE	 *return_code )
{
	(void) lock_level;
	*return_code = NOT_AVAILABLE;
}

/********************************************************************
* SERVICE  UNLOCK_PREEMPTION
*********************************************************************/
void UNLOCK_PREEMPTION (LOCK_LEVEL_TYPE	*lock_level,
								RETURN_CODE_TYPE  *return_code )

{
	(void) lock_level;
	*return_code = NOT_AVAILABLE;
}


#endif

#endif
