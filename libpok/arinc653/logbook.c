
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

#include <types.h>
#include <arinc653/types.h>
#include <arinc653/logbook.h>

/********************************************************************
* SERVICE  CREATE_LOGBOOK
********************************************************************/
void CREATE_LOGBOOK
		(const LOGBOOK_NAME_TYPE 	/* in */	LOGBOOK_NAME, 
		const MESSAGE_SIZE_TYPE 	/* in */	MAX_MESSAGE_SIZE, 
		const MESSAGE_RANGE_TYPE 	/* in */	MAX_NB_LOGGED_MESSAGE, 
		const MESSAGE_RANGE_TYPE 	/* in */	MAX_NB_IN_PROGRESS_MESSAGE, 
		LOGBOOK_ID_TYPE * const 	/* out */	LOGBOOK_ID, 
		RETURN_CODE_TYPE * const 	/* out */	RETURN_CODE)
{

	(void) LOGBOOK_NAME;
	(void) MAX_MESSAGE_SIZE;
	(void) MAX_NB_LOGGED_MESSAGE;
	(void) MAX_NB_IN_PROGRESS_MESSAGE;
	(void) LOGBOOK_ID;
	*RETURN_CODE = NO_ERROR;
}


/********************************************************************
* SERVICE  READ_LOGBOOK
********************************************************************/
void READ_LOGBOOK
		(const LOGBOOK_ID_TYPE 		/* in */	LOGBOOK_ID, 
		const MESSAGE_RANGE_TYPE 	/* in */	LOGBOOK_ENTRY, 
		MESSAGE_ADDR_TYPE 			/* in out */	MESSAGE_ADDR, 
		MESSAGE_SIZE_TYPE * const 	/* out */	LENGTH, 
		WRITE_STATUS_TYPE * const 	/* out */	WRITE_STATUS, 
		RETURN_CODE_TYPE * const 	/* out */	RETURN_CODE)
{

	(void) LOGBOOK_ID;
	(void) LOGBOOK_ENTRY;
	(void) MESSAGE_ADDR;
	(void) LENGTH;
	(void) WRITE_STATUS;
	*RETURN_CODE = NO_ERROR;
}


/********************************************************************
* SERVICE  WRITE_LOGBOOK
********************************************************************/
void WRITE_LOGBOOK
		(const LOGBOOK_ID_TYPE 	/* in */	LOGBOOK_ID, 
		const MESSAGE_ADDR_TYPE /* in */	MESSAGE_ADDR, 
		const MESSAGE_SIZE_TYPE /* in */	LENGTH, 
		RETURN_CODE_TYPE * const/* out */	RETURN_CODE)
{
	(void) LOGBOOK_ID;
	(void) MESSAGE_ADDR;
	(void) LENGTH;
	*RETURN_CODE = NO_ERROR;
}


/********************************************************************
* SERVICE  CLEAR_LOGBOOK
*********************************************************************/
void CLEAR_LOGBOOK
		(const LOGBOOK_ID_TYPE 		/* in */	LOGBOOK_ID, 
		RETURN_CODE_TYPE * const 	/* out */	RETURN_CODE)
{	
	(void) LOGBOOK_ID;
	*RETURN_CODE = NO_ERROR;
}


/********************************************************************
* SERVICE  GET_LOGBOOK_ID
*********************************************************************/
void GET_LOGBOOK_ID
		(const LOGBOOK_NAME_TYPE 	/* in */	LOGBOOK_NAME, 
		LOGBOOK_ID_TYPE * const 	/* out */	LOGBOOK_ID, 
		RETURN_CODE_TYPE * const 	/* out */	RETURN_CODE)
{
	(void) LOGBOOK_NAME;
	(void) LOGBOOK_ID;

	 *RETURN_CODE = NO_ERROR;
}


/********************************************************************
* SERVICE  GET_LOGBOOK_STATUS
********************************************************************/
void GET_LOGBOOK_STATUS
		(const LOGBOOK_ID_TYPE 		/* in */	LOGBOOK_ID, 
		LOGBOOK_STATUS_TYPE * const /* out */	LOGBOOK_STATUS, 
		RETURN_CODE_TYPE * const 	/* out */	RETURN_CODE){

	(void) LOGBOOK_ID;
	(void) LOGBOOK_STATUS;
	*RETURN_CODE = NO_ERROR;

}

