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
 */


#include <arinc653/types.h>

/* SAMPLING PORT constant and type definitions and management services*/ 
 
#ifndef APEX_LOGBOOK 
#define APEX_LOGBOOK 

typedef  NAME_TYPE					 LOGBOOK_NAME_TYPE;

typedef  APEX_INTEGER				 LOGBOOK_ID_TYPE;

typedef  enum { ABORTED = 0, IN_PROGRESS = 1, COMPLETE = 2 } WRITE_STATUS_TYPE;

typedef
	struct {
		MESSAGE_SIZE_TYPE			MAX_MESSAGE_SIZE;
										  /* maximum size of message				*/
		MESSAGE_RANGE_TYPE		  MAX_NB_LOGGED_MESSAGE;
										  /* maximum number of messages in logbook	*/
		MESSAGE_RANGE_TYPE		  MAX_NB_IN_PROGRESS_MESSAGE;
										  /* maximum number of messages in progress	*/
		MESSAGE_RANGE_TYPE		  NB_LOGGED_MESSAGE;
										  /* current number of messages in logbook	*/
		MESSAGE_RANGE_TYPE		  NB_IN_PROGRESS_MESSAGE;
										  /* current number of messages in progress	*/
		MESSAGE_RANGE_TYPE		  NB_ABORTED_MESSAGE;
										  /* current number of aborted messages		*/
	} LOGBOOK_STATUS_TYPE;



extern void CREATE_LOGBOOK
		(const LOGBOOK_NAME_TYPE 	/* in */	LOGBOOK_NAME, 
		const MESSAGE_SIZE_TYPE 	/* in */	MAX_MESSAGE_SIZE, 
		const MESSAGE_RANGE_TYPE 	/* in */	MAX_NB_LOGGED_MESSAGE, 
		const MESSAGE_RANGE_TYPE 	/* in */	MAX_NB_IN_PROGRESS_MESSAGE, 
		LOGBOOK_ID_TYPE * const 	/* out */	LOGBOOK_ID, 
		RETURN_CODE_TYPE * const 	/* out */	RETURN_CODE);

extern void READ_LOGBOOK
		(const LOGBOOK_ID_TYPE 		/* in */	LOGBOOK_ID, 
		const MESSAGE_RANGE_TYPE 	/* in */	LOGBOOK_ENTRY, 
		MESSAGE_ADDR_TYPE 			/* in out */MESSAGE_ADDR, 
		MESSAGE_SIZE_TYPE * const 	/* out */	LENGTH, 
		WRITE_STATUS_TYPE * const 	/* out */	WRITE_STATUS, 
		RETURN_CODE_TYPE * const 	/* out */	RETURN_CODE);

extern void WRITE_LOGBOOK
		(const LOGBOOK_ID_TYPE 	/* in */	LOGBOOK_ID, 
		const MESSAGE_ADDR_TYPE /* Array */	/* in */	MESSAGE_ADDR, 
		const MESSAGE_SIZE_TYPE 	/* in */	LENGTH, 
		RETURN_CODE_TYPE * const 	/* out */	RETURN_CODE);

extern void CLEAR_LOGBOOK
		(const LOGBOOK_ID_TYPE 	/* in */	LOGBOOK_ID, 
		RETURN_CODE_TYPE * const 	/* out */	RETURN_CODE);

extern void GET_LOGBOOK_ID
		(const LOGBOOK_NAME_TYPE 	/* in */	LOGBOOK_NAME, 
		LOGBOOK_ID_TYPE * const 	/* out */	LOGBOOK_ID, 
		RETURN_CODE_TYPE * const 	/* out */	RETURN_CODE);

extern void GET_LOGBOOK_STATUS
		(const LOGBOOK_ID_TYPE 	/* in */	LOGBOOK_ID, 
		LOGBOOK_STATUS_TYPE * const 	/* out */	LOGBOOK_STATUS, 
		RETURN_CODE_TYPE * const 	/* out */	RETURN_CODE);

#endif
