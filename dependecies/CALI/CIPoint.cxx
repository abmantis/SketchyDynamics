/*--------------------------------------------------------- -*- Mode: C++ -*- +
| Module: CIPoint.cxx
+-----------------------------------------------------------------------------+
| Description: 
| 
| Notes:       
|
| Author: Manuel Joao Fonseca
|	  e-mail: mjf@inesc-id.pt
|
| Date: April 99
+-----------------------------------------------------------------------------+
|
| Copyright (C) 1998, 1999, 2000 Manuel João da Fonseca
|
| This program is free software; you can redistribute it and/or
| modify it under the terms of the GNU General Public License
| as published by the Free Software Foundation; either version 2
| of the License, or any later version.
| 
| This program is distributed in the hope that it will be useful,
| but WITHOUT ANY WARRANTY; without even the implied warranty of
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
| GNU General Public License for more details.
| 
| You should have received a copy of the GNU General Public License
| along with this program; if not, write to the Free Software
| Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
| 
+----------------------------------------------------------------------------*/

#include "../dependecies/CALI/CIPoint.h"

#ifdef WIN32 	// For Windows
CIPoint& CIPoint::operator= (CIPoint& point)
#else 		// For Linux
CIPoint& CIPoint::operator= (const CIPoint& point)
#endif
{
    x=point.x; 
    y=point.y; 
    _time=point.getTime(); 

    return *this; 
}
