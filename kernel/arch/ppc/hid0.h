/*  
 * Copyright (c) 2012  University of Padua, Department of Mathematics  
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


/**
 * POK definitions to hanlde the HID0 registers
 */

#ifndef __POK_PPC_HID0_H__
#define __POK_PPC_HID0_H__

#define HID0 		1008

#define HID0_NAP	(1 << 22)
#define HID0_SLEEP	(1 << 21)

#define HID0_ICE	(1 << 15)
#define HID0_DCE	(1 << 14)
#define HID0_ILOCK	(1 << 13)
#define HID0_DLOCK	(1 << 12)
#define HID0_ICFI	(1 << 11)
#define HID0_DCFI	(1 << 10)

#define HID0_INIT_ICACHE (HID0_ICE | HID0_ICFI)
#define HID0_INIT_DCACHE (HID0_DCE | HID0_DCFI)

#endif

